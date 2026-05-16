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

} // namespace

extern "C" void app_main() {
  bsp::Adc<2> adc(ADC_UNIT_1, {
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
                              });

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    uint32_t voltage0 = adc.get_voltage_mV(0);
    uint32_t voltage1 = adc.get_voltage_mV(1);
  }
}
