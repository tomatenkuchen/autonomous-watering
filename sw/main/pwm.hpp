/**
 * @file pwm.hpp
 * @brief PWM control module
 */

#pragma once

#include "driver/ledc.h"
#include "hal/ledc_types.h"
#include <cstdint>

namespace bsp::pwm {

class Pwm {
public:
  Pwm();

  /**
   * @brief set PWM duty cycle for usb
   * @param duty PWM duty cycle in fraction (0 -> 0%, 65535 -> 100%)
   */
  void set_usb_duty(uint16_t duty);

  /**
   * @brief set PWM duty cycle for solar
   * @param duty PWM duty cycle in fraction (0 -> 0%, 65535 -> 100%)
   */
  void set_solar_duty(uint16_t duty);

  /**
   * @brief set PWM duty cycle for conductivity
   * @param duty PWM duty cycle in fraction (0 -> 0%, 65535 -> 100%)
   */
  void set_conductivity_duty(uint16_t duty);

private:
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = LEDC_TIMER_13_BIT, // Set duty resolution to 13 bits
      .timer_num = LEDC_TIMER_0,
      .freq_hz = 40'000,
      .clk_cfg = LEDC_AUTO_CLK,
      .deconfigure = false,
  };

  ledc_channel_config_t ledc_channel_usb = {
      .gpio_num = 18,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_0,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
      .hpoint = 0,
      .sleep_mode = LEDC_SLEEP_MODE_INVALID,
      .flags =
          {
              .output_invert = 0,
          },
      .deconfigure = false,
  };

  ledc_channel_config_t ledc_channel_solar = {
      .gpio_num = 20,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_1,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
      .hpoint = 0,
      .sleep_mode = LEDC_SLEEP_MODE_INVALID,
      .flags =
          {
              .output_invert = 0,
          },
      .deconfigure = false,
  };

  ledc_channel_config_t ledc_channel_conductivity = {
      .gpio_num = 22,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_2,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
      .hpoint = 0,
      .sleep_mode = LEDC_SLEEP_MODE_INVALID,
      .flags =
          {
              .output_invert = 0,
          },
      .deconfigure = false,
  };
};

} // namespace bsp::pwm