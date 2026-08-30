/**
 * @file main.cpp
 * @brief entry point of executable
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "power.hpp"
#include "valves.hpp"

namespace
{

const static char *TAG = "main";
bsp::Power *power_ptr = nullptr;

auto charge_battery() -> void
{
    auto const v_bat = power_ptr->get_voltage_mV(bsp::Power::AdcInputs::v_battery);
    auto const v_solar = power_ptr->get_voltage_mV(bsp::Power::AdcInputs::v_solar);

    // calculate duty for power transfer
    float const i_bat_ref_mA = 100.0;
    float const L_uH = 22.0;
}

} // namespace

extern "C" void app_main()
{
    bsp::Power power;
    power_ptr = &power;

    std::uint16_t v_cond_out_mV = 0;

    bsp::valves::Valves valves;

    std::uint8_t valve_cnt = 0;

    while (true)
    {
        ESP_LOGI(TAG, "main loop");
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "USB voltage online: %s", power.is_usb_voltage_online() ? "yes" : "no");

        ESP_LOGI(TAG, "voltages: \n\tsolar: %d mV\n\tbattery: %dmV",
                 power.get_voltage_mV(bsp::Power::AdcInputs::v_solar),
                 power.get_voltage_mV(bsp::Power::AdcInputs::v_battery));

        ESP_LOGI(TAG, "currents: \n\tsolar: %d mA\n\tconductivity: %dmA\n\tusb: %d mA",
                 power.get_voltage_mV(bsp::Power::AdcInputs::i_solar),
                 power.get_voltage_mV(bsp::Power::AdcInputs::i_conductivity),
                 power.get_voltage_mV(bsp::Power::AdcInputs::i_usb));

        // conductivity
        v_cond_out_mV += 1024;
        power.set_conductivity_voltage_mV(v_cond_out_mV);

        ESP_LOGI(TAG, "conductivity voltage: %d\n", v_cond_out_mV);

        // valves
        bool const v1 = valve_cnt & 1;
        bool const v2 = (valve_cnt >> 1) & 2;
        valves.enable_valve_1(v1);
        valves.enable_valve_2(v2);
        ESP_LOGI(TAG, "valve1: %s", v1 ? "on" : "off");
        ESP_LOGI(TAG, "valve2: %s", v2 ? "on" : "off");
        valve_cnt++;

        // power transfer to battery
    }
}
