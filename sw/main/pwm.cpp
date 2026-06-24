#include "pwm.hpp"

namespace bsp::pwm
{

Pwm::Pwm()
{
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

Pwm::~Pwm()
{
}

void Pwm::set_duty(uint16_t duty)
{
    // Set duty to 50%
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty >> (LEDC_TIMER_13_BIT - 8)));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

} // namespace bsp::pwm
