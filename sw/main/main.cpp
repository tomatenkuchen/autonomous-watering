/**
 * @file main.cpp
 * @brief entry point of executable
 */

#include "adc.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "pwm.hpp"
#include "soc/soc_caps.h"
#include "valves.hpp"
#include <array>

namespace {

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

enum AdcInputs {
  i_usb,
  i_solar,
  v_solar,
  i_conductivity,
  v_battery,
};

auto is_usb_voltage_online() -> bool {
  gpio_config_t v_usb_gpio = {
      .pin_bit_mask = 1 << 19,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_ENABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&v_usb_gpio));
  return gpio_get_level(GPIO_NUM_19) == 1;
}

} // namespace

extern "C" void app_main() {
  bsp::adc::Adc<5> adc(ADC_UNIT_1, adc_channels);

  bsp::pwm::Pwm pwm;

  uint16_t duty = 1000;
  bool valves = false;

  bsp::valves::Valves valves;

  while (true) {
    ESP_LOGI(TAG, "main loop");
    vTaskDelay(pdMS_TO_TICKS(1000));
    adc.get_voltage_mV(i_usb);
    adc.get_voltage_mV(i_solar);
    adc.get_voltage_mV(v_solar);
    adc.get_voltage_mV(i_conductivity);
    adc.get_voltage_mV(v_battery);

    ESP_LOGI(TAG, "USB voltage online: %s",
             is_usb_voltage_online() ? "yes" : "no");

    // pwm.set_usb_duty(duty);
    // pwm.set_solar_duty(duty);
    pwm.set_conductivity_duty(duty);

    // duty += 1000;

    valves.enable_valve_1(valves);
    valves.enable_valve_2(valves);
    valves = !valves;
  }
}
