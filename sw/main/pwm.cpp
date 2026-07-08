#include "pwm.hpp"

namespace bsp::pwm
{

Pwm::Pwm()
{
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_usb));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_solar));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_conductivity));
}

auto Pwm::set_usb_duty(uint16_t duty) -> void
{
    set_duty(ledc_channel_usb.speed_mode, ledc_channel_usb.channel, duty);
}

auto Pwm::set_solar_duty(uint16_t duty) -> void
{
    set_duty(ledc_channel_solar.speed_mode, ledc_channel_solar.channel, duty);
}

auto Pwm::set_conductivity_duty(uint16_t duty) -> void
{
    set_duty(ledc_channel_conductivity.speed_mode, ledc_channel_conductivity.channel, duty);
}

auto Pwm::set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, std::uint16_t duty) -> void
{
    // denormalize duty from 16 bit to the actual duty resolution
    std::uint32_t duty_in = duty >> (16 - (int)ledc_timer.duty_resolution);
    // load duty
    ESP_ERROR_CHECK(ledc_set_duty(speed_mode, channel, duty_in));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(speed_mode, channel));
}

} // namespace bsp::pwm
