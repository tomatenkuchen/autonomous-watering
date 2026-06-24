/**
 * @file adc.hpp
 * @brief adc measurement module
 */

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include <array>
#include <cstdint>
#include <ranges>

namespace bsp::adc
{

struct ChannelConfig
{
    adc_channel_t channel;
    adc_atten_t atten;
    adc_bitwidth_t bitwidth;
};

template <uint8_t channels> class Adc
{
  public:
    /**
     * @brief constructor
     * @param unit ADC unit
     * @param adc_channels Array of channel configurations
     */
    Adc(adc_unit_t unit, std::array<ChannelConfig, channels> adc_channels);
    ~Adc();

    /**
     * @brief get voltage in mV of specified channel
     * @param channel_no Channel number
     * @return Voltage in mV
     */
    auto get_voltage_mV(uint8_t channel_no) -> int;

  private:
    static constexpr const char *TAG = "Adc";
    adc_oneshot_unit_handle_t adc_handle = nullptr;
    std::array<ChannelConfig, channels> adc_channels;
    std::array<adc_cali_handle_t, channels> adc_cali_handles;
};

template <uint8_t channels>
Adc<channels>::Adc(adc_unit_t unit, std::array<ChannelConfig, channels> adc_channels)
    : adc_handle(nullptr),
      adc_channels(adc_channels)
{
    // init adc peripheal
    ESP_LOGI(TAG, "init adc peripheal");
    adc_oneshot_unit_handle_t adc1_handle = nullptr;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = unit,
        .clk_src = ADC_DIGI_CLK_SRC_XTAL,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    adc_handle = adc1_handle;

    // init adc channels
    for (auto channel : adc_channels)
    {
        adc_oneshot_chan_cfg_t config = {
            .atten = channel.atten,
            .bitwidth = channel.bitwidth,
        };
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, channel.channel, &config));
    }

    // calibration
    for (auto cal : std::views::zip(adc_channels, adc_cali_handles))
    {
        auto [channel, handle] = cal;

        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel.channel,
            .atten = channel.atten,
            .bitwidth = channel.bitwidth,
        };

        auto const ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Calibration Success");
        }
        else if (ret == ESP_ERR_NOT_SUPPORTED)
        {
            ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
        }
        else
        {
            ESP_LOGE(TAG, "Invalid arg or no memory");
        }
    }
}

template <uint8_t channels> Adc<channels>::~Adc()
{
    // deinit adc
    if (adc_handle)
    {
        ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
    }

    // remove calibration
    for (auto cali : adc_cali_handles)
    {
        ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
        ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(cali));
    }
}

template <uint8_t channels> auto Adc<channels>::get_voltage_mV(uint8_t channel_no) -> int
{
    if (channel_no >= channels)
    {
        ESP_LOGE(TAG, "Invalid channel number");
        return 0;
    }

    // get raw data
    int adc_raw = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, adc_channels[channel_no].channel, &adc_raw));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, adc_channels[channel_no].channel, adc_raw);

    // convert to voltage
    int voltage = 0;
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handles[channel_no], adc_raw, &voltage));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, adc_channels[channel_no].channel, voltage);

    return voltage;
}

} // namespace bsp::adc