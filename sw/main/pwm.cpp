#include "pwm.hpp"

namespace bsp::pwm {

Pwm::Pwm() {
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_usb));
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_solar));
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_conductivity));
}

void Pwm::set_usb_duty(uint16_t duty) {
  // Set duty to 50%
  ESP_ERROR_CHECK(ledc_set_duty(ledc_channel_usb.speed_mode,
                                ledc_channel_usb.channel,
                                duty >> (LEDC_TIMER_13_BIT - 8)));
  // Update duty to apply the new value
  ESP_ERROR_CHECK(
      ledc_update_duty(ledc_channel_usb.speed_mode, ledc_channel_usb.channel));
}

void Pwm::set_solar_duty(uint16_t duty) {
  // Set duty to 50%
  ESP_ERROR_CHECK(ledc_set_duty(ledc_channel_solar.speed_mode,
                                ledc_channel_solar.channel,
                                duty >> (LEDC_TIMER_13_BIT - 8)));
  // Update duty to apply the new value
  ESP_ERROR_CHECK(ledc_update_duty(ledc_channel_solar.speed_mode,
                                   ledc_channel_solar.channel));
}

void Pwm::set_conductivity_duty(uint16_t duty) {
  // Set duty to 50%
  ESP_ERROR_CHECK(ledc_set_duty(ledc_channel_conductivity.speed_mode,
                                ledc_channel_conductivity.channel,
                                duty >> (LEDC_TIMER_13_BIT - 8)));
  // Update duty to apply the new value
  ESP_ERROR_CHECK(ledc_update_duty(ledc_channel_conductivity.speed_mode,
                                   ledc_channel_conductivity.channel));
}

} // namespace bsp::pwm
