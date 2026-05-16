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

namespace bsp {

template <uint8_t channels> class Adc {
public:
  struct ChannelConfig {
    adc_channel_t channel;
    adc_atten_t atten;
    adc_bitwidth_t bitwidth;
  };

  Adc(adc_unit_t unit, std::array<ChannelConfig, channels> adc_channels);
  ~Adc();

private:
  static constexpr const char *TAG = "Adc";
  adc_oneshot_unit_handle_t adc_handle;
  std::array<ChannelConfig, channels> adc_channels;
  std::array<adc_cali_handle_t, channels> adc_cali_handles;
};

template <uint8_t channels>
Adc<channels>::Adc(adc_unit_t unit,
                   std::array<ChannelConfig, channels> adc_channels)
    : adc_channels(adc_channels) {

  // init adc peripheal
  ESP_LOGI(TAG, "init adc peripheal");
  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = unit,
      .clk_src = SOC_MOD_CLK_RC_FAST,
      .ulp_mode = ADC_ULP_MODE_RISCV,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

  // init adc channels
  for (auto channel : adc_channels) {
    adc_oneshot_chan_cfg_t config = {
        .atten = channel.atten,
        .bitwidth = channel.bitwidth,
    };
    ESP_ERROR_CHECK(
        adc_oneshot_config_channel(adc1_handle, channel.channel, &config));
  }

  // calibration
  for (auto cal : std::views::zip(adc_channels, adc_cali_handles)) {
    auto [channel, handle] = cal;

    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = channel.atten,
        .bitwidth = channel.bitwidth,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED) {
      ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
      ESP_LOGE(TAG, "Invalid arg or no memory");
    }
  }
}

template <uint8_t channels>
auto Adc<channels>::get_voltage_mV(uint8_t channel_no) -> uint32_t {
  if (channel_no >= channels) {
    ESP_LOGE(TAG, "Invalid channel number");
    return 0;
  }

  // get raw data
  uint32_t adc_raw = 0;
  ESP_ERROR_CHECK(
      adc_oneshot_read(adc_handle, adc_channels[channel_no].channel, &adc_raw));
  ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1,
           adc_channels[channel_no].channel, adc_raw);

  // convert to voltage
  uint32_t voltage = 0;
  ESP_ERROR_CHECK(
      adc_cali_raw_to_voltage(adc_cali_handles[channel_no], adc_raw, &voltage));
  ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1,
           adc_channels[channel_no].channel, voltage);
}

template <uint8_t channels> Adc<channels>::~Adc() {
  // deinit adc
  ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));

  // remove calibration
  for (auto cali : adc_cali_handles) {
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(cali));
  }
}

} // namespace bsp