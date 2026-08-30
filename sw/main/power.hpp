/**
 * @file power.hpp
 * @brief power distribution on board
 * @license GPLv2
 */

#pragma once

#include "adc.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "pwm.hpp"
#include "soc/soc_caps.h"
#include <array>
#include <cstdint>

namespace bsp
{
class Power
{
  public:
    enum AdcInputs
    {
        i_usb,
        i_solar,
        v_solar,
        i_conductivity,
        v_battery,
    };

    Power();

    auto get_voltage_mV(AdcInputs input) -> std::uint16_t;

    auto get_current_mA(AdcInputs input) -> std::uint16_t;

    auto set_conductivity_voltage_mV(std::uint16_t output_mV) -> void;

    auto is_usb_voltage_online() -> bool;

    auto set_usb_current_ref_mA(std::int32_t duty) -> void;

    auto set_solar_current_ref_mA(std::int32_t duty) -> void;

  private:
    constexpr static std::array<adc::ChannelConfig, 5> adc_channels = {
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

    bsp::adc::Adc<5> adc;

    bsp::pwm::Pwm pwm;
};
} // namespace bsp
