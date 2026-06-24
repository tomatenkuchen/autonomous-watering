/**
 * @file main.cpp
 * @brief entry point of executable
 */

#include "adc.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "pwm.hpp"
#include "soc/soc_caps.h"
#include <array>

namespace
{

const static char *TAG = "main";

std::array<bsp::adc::ChannelConfig, 5> adc_channels = {
    {
        {
            .channel = ADC_CHANNEL_2,
            .atten = ADC_ATTEN_DB_0,
            .bitwidth = ADC_BITWIDTH_12,
        },
        {
            .channel = ADC_CHANNEL_3,
            .atten = ADC_ATTEN_DB_0,
            .bitwidth = ADC_BITWIDTH_12,
        },
        {
            .channel = ADC_CHANNEL_4,
            .atten = ADC_ATTEN_DB_0,
            .bitwidth = ADC_BITWIDTH_12,
        },
        {
            .channel = ADC_CHANNEL_5,
            .atten = ADC_ATTEN_DB_0,
            .bitwidth = ADC_BITWIDTH_12,
        },
        {
            .channel = ADC_CHANNEL_6,
            .atten = ADC_ATTEN_DB_0,
            .bitwidth = ADC_BITWIDTH_12,
        },
    },
};

enum AdcInputs
{
    i_usb,
    i_solar,
    v_solar,
    i_conductivity,
    v_battery,
};

} // namespace

extern "C" void app_main()
{
    bsp::adc::Adc<3> adc(ADC_UNIT_1, adc_channels);

    while (true)
    {
        ESP_LOGI(TAG, "main loop");
        vTaskDelay(pdMS_TO_TICKS(1000));
        adc.get_voltage_mV(i_usb);
        adc.get_voltage_mV(i_solar);
        adc.get_voltage_mV(v_solar);
        adc.get_voltage_mV(i_conductivity);
        adc.get_voltage_mV(v_battery);
    }
}
