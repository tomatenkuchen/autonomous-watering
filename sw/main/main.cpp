/**
 * @file main.cpp
 * @brief entry point of executable
 */

#include "adc.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "soc/soc_caps.h"
#include <array>

namespace {

const static char *TAG = "main";

std::array<bsp::adc::ChannelConfig, 3> adc_channels = {
    {
        {
            .channel = ADC_CHANNEL_0,
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_12,
        },
        {
            .channel = ADC_CHANNEL_1,
            .atten = ADC_ATTEN_DB_2_5,
            .bitwidth = ADC_BITWIDTH_12,
        },
        {
            .channel = ADC_CHANNEL_2,
            .atten = ADC_ATTEN_DB_6,
            .bitwidth = ADC_BITWIDTH_12,
        },
    },
};

} // namespace

extern "C" void app_main() {
  bsp::adc::Adc<3> adc(ADC_UNIT_1, adc_channels);

  while (true) {
    ESP_LOGI(TAG, "main loop");
    vTaskDelay(pdMS_TO_TICKS(1000));
    adc.get_voltage_mV(0);
    adc.get_voltage_mV(1);
    adc.get_voltage_mV(2);
  }
}
