/**
 * @file valves.hpp
 * @brief valve control functionality
 */

#pragma once

#include "driver/gpio.h"

namespace bsp::valves {

class Valves {
public:
  Valves() { ESP_ERROR_CHECK(gpio_config(&valve_gpio)); }

  auto enable_valve_1(bool state) -> void {
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_11, state ? 1 : 0));
  }

  auto enable_valve_2(bool state) -> void {
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_15, state ? 1 : 0));
  }

private:
  gpio_config_t valve_gpio = {
      .pin_bit_mask = (1 << 11) | (1 << 15),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_ENABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
};

} // namespace bsp::valves