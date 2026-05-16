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

namespace {

const static char *TAG = "main";

std::array<bsp::adc::ChannelConfig, 2> adc_channels = {
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
    },
};

} // namespace

extern "C" void app_main() {
  bsp::adc::Adc<2> adc(ADC_UNIT_1, adc_channels);

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    int voltage0 = adc.get_voltage_mV(0);
    int voltage1 = adc.get_voltage_mV(1);
  }
}
