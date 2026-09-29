/**
 * @file main.cpp
 * @brief entry point of executable
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "power.hpp"
#include "valves.hpp"

namespace {

const static char *TAG = "main";
bsp::Power *power_ptr = nullptr;

auto charge_battery() -> void {
  auto const v_bat =
      power_ptr->get_voltage_mV(bsp::Power::AdcInputs::v_battery);
  auto const v_solar =
      power_ptr->get_voltage_mV(bsp::Power::AdcInputs::v_solar);

  // calculate duty for power transfer
  float const i_bat_ref_mA = 100.0;
  float const L_uH = 22.0;
}

} // namespace

extern "C" void app_main() {
  bsp::Power power;
  power_ptr = &power;

  std::uint16_t v_cond_out_mV = 0;

  bsp::valves::Valves valves;

  std::uint8_t valve_cnt = 0;

  while (true) {
    ESP_LOGI(TAG, "main loop");
    vTaskDelay(pdMS_TO_TICKS(10));

    // power transfer to battery
    power.set_usb_current_ref_mA(100);
  }
}
