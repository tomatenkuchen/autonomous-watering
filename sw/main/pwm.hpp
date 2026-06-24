/**
 * @file pwm.hpp
 * @brief PWM control module
 */

#pragma once

#include "driver/ledc.h"
#include <cstdint>

namespace bsp::pwm
{

class Pwm
{
  public:
    Pwm();
    ~Pwm();

    /**
     * @brief set PWM duty cycle
     * @param duty_percent PWM duty cycle in fraction (0 -> 0%, 65535 -> 100%)
     */
    void set_duty(uint16_t duty);

  private:
    ledc_channel_t channel;

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT, // Set duty resolution to 13 bits
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 40'000,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .gpio_num = 18,
        .duty = 0,
        .hpoint = 0,
    };
};

} // namespace bsp::pwm