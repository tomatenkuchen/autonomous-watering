#include "power.hpp"
#include "adc.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include <algorithm>
#include <array>
#include <cstdint>

namespace
{
const static char *TAG = "power";
}

namespace bsp
{

Power::Power()
    : adc(ADC_UNIT_1, adc_channels)
{
    ESP_LOGI(TAG, "power object constructed");
}

auto Power::get_voltage_mV(AdcInputs input) -> std::uin16_t
{
    switch (input)
    {
    case AdcInputs::v_solar: {
        float const R1 = 220'000.0f;
        float const R2 = 10'000.0f;
        auto const v_solar_meas_mV = adc.get_voltage_mV(v_solar);
        auto const v_solar_mV = v_solar_meas_mV * (R1 + R2) / R2;
        return static_cast<uint16_t>(v_solar_mV);
    }
    case AdcInputs::v_battery: {
        float const R1 = 39'000.0f;
        float const R2 = 10'000.0f;
        auto const v_battery_meas_mV = adc.get_voltage_mV(v_battery);
        auto const v_battery_mV = v_battery_meas_mV * (R1 + R2) / R2;
        return static_cast<uint16_t>(v_battery_mV);
    }
    case AdcInputs::i_conductivity:
    case AdcInputs::i_usb:
    case AdcInputs::i_solar:
    default:
        return 0;
    }
}

auto Power::get_current_mA(AdcInputs input) -> std::uint16_t
{
    float const R_sh = 0.1f;
    float amp = 11.0f;
    auto const V_meas_mV = adc.get_voltage_mV(input);
    auto const V_sh_mV = V_meas_mV / amp;
    auto const I_sh_mA = V_sh_mV / R_sh;
    return static_cast<std::uint16_t>(I_sh_mA);
}

auto Power::set_conductivity_voltage_mV(std::uint16_t output_mV) -> void
{
    std::uint16_t const V_max_mV = 24'000;
    // limit
    auto const V_out_mV = std::min(V_max_mV, output_mV);

    auto const duty = V_out_mV * 65535UL / V_max_mV;

    pwm.set_conductivity_duty(duty);
}

auto Power::is_usb_voltage_online() -> bool
{
    gpio_config_t v_usb_gpio = {
        .pin_bit_mask = 1 << 21,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&v_usb_gpio));
    return gpio_get_level(GPIO_NUM_21) == 1;
}

} // namespace bsp
