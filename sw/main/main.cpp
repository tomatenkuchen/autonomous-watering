/**
 * @file main.cpp
 * @brief entry point of executable
 */

#include "adc.hpp"
#include "driver/temperature_sensor.h"
#include "esp_log.h"
#include "freertos.hpp"
#include "power.hpp"
#include "server.hpp"
#include "valves.hpp"
#include <chrono>

using namespace std::chrono_literals;

namespace {

const static char *TAG = "main";

temperature_sensor_handle_t temp_sensor = nullptr;

// Initialize Built-in Temperature Sensor
auto init_temp_sensor() -> void {
  temperature_sensor_config_t temp_cfg =
      TEMPERATURE_SENSOR_CONFIG_DEFAULT(20, 50);
  ESP_ERROR_CHECK(temperature_sensor_install(&temp_cfg, &temp_sensor));
  ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));
}

} // namespace

extern "C" void app_main() {
  init_temp_sensor();

  bsp::Power power;

  mqtt_server::ServerConfig server_config{
      .ip_address = {192, 168, 1, 100},
      .port = 1883,
  };
  mqtt_server::WifiCredentials wifi_credentials{
      .ssid = "YOUR_WIFI_SSID",
      .password = "YOUR_WIFI_PASSWORD",
  };

  mqtt_server::Iface iface{
      .get_temperature_celsius =
          [] {
            float t;
            temperature_sensor_get_celsius(temp_sensor, &t);
            return t;
          },
      .get_battery_voltage_mV =
          [&power] {
            return power.get_voltage_mV(bsp::Power::AdcInputs::v_battery);
          },
  };

  mqtt_server::start(server_config, wifi_credentials, iface);

  while (true) {
    // power transfer to battery
    power.set_solar_current_ref_mA(1);

    freertos::delay(10s);
  }
}
