#include "server.hpp"
#include "esp_check.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos.hpp"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include <chrono>
#include <cstdio>
#include <cstring>
#include <format>

using namespace std::chrono_literals;

namespace mqtt_server {

namespace {

const char *TAG = "mqtt_server";
esp_mqtt_client_handle_t mqtt_client = nullptr;
freertos::EventGroup wifi_event_group;
Iface iface;
constexpr EventBits_t WIFI_CONNECTED_BIT = BIT0;

auto wifi_event_handler(void *handler_args, esp_event_base_t event_base,
                        int32_t event_id, void *event_data) -> void {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    const esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to start Wi-Fi connection: %s",
               esp_err_to_name(err));
    }
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    wifi_event_group.clear_bits(WIFI_CONNECTED_BIT);
    ESP_LOGW(TAG, "Wi-Fi disconnected; reconnecting");
    const esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Wi-Fi reconnect failed: %s", esp_err_to_name(err));
    }
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    const auto *event = static_cast<ip_event_got_ip_t *>(event_data);
    ESP_LOGI(TAG, "Wi-Fi connected, IP: " IPSTR, IP2STR(&event->ip_info.ip));
    wifi_event_group.set_bits(WIFI_CONNECTED_BIT);
  }
}

auto wifi_init_sta(WifiCredentials const &creds) -> esp_err_t {
  if (creds.ssid.empty()) {
    return ESP_ERR_INVALID_ARG;
  }

  wifi_config_t wifi_config{};

  // Copy SSID and password into wifi_config
  const size_t ssid_length = creds.ssid.length();
  const size_t password_length = creds.password.length();

  // check if the lengths are within the limits of the wifi_config struct
  if (ssid_length > sizeof(wifi_config.sta.ssid) ||
      password_length > sizeof(wifi_config.sta.password)) {
    return ESP_ERR_INVALID_SIZE;
  }

  std::memcpy(wifi_config.sta.ssid, creds.ssid.c_str(), ssid_length);
  std::memcpy(wifi_config.sta.password, creds.password.c_str(),
              password_length);

  wifi_config.sta.threshold.authmode =
      password_length == 0 ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
  wifi_config.sta.pmf_cfg.capable = true;

  ESP_RETURN_ON_ERROR(esp_netif_init(), TAG, "Failed to initialize netif");

  esp_err_t err = esp_event_loop_create_default();
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
    return err;
  }
  if (esp_netif_create_default_wifi_sta() == nullptr) {
    return ESP_FAIL;
  }

  wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_RETURN_ON_ERROR(esp_wifi_init(&init_config), TAG,
                      "Failed to initialize Wi-Fi");

  ESP_RETURN_ON_ERROR(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                 wifi_event_handler, nullptr),
                      TAG, "Failed to register Wi-Fi event handler");
  ESP_RETURN_ON_ERROR(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                 wifi_event_handler, nullptr),
                      TAG, "Failed to register IP event handler");
  ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG,
                      "Failed to set station mode");
  ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wifi_config), TAG,
                      "Failed to configure station");
  ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "Failed to start Wi-Fi");

  EventBits_t const bits =
      wifi_event_group.wait_bits(WIFI_CONNECTED_BIT, false, true, 30s);
  if ((bits & WIFI_CONNECTED_BIT) == 0) {
    ESP_LOGE(TAG, "Timed out waiting for Wi-Fi connection");
    return ESP_ERR_TIMEOUT;
  }
  return ESP_OK;
}

// MQTT Event Handler
auto mqtt_event_handler(void *handler_args, esp_event_base_t base,
                        int32_t event_id, void *event_data) -> void {

  auto const event = static_cast<esp_mqtt_event_handle_t>(event_data);

  switch ((esp_mqtt_event_id_t)event_id) {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT Connected to Broker");
    break;
  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGW(TAG, "MQTT Disconnected");
    break;
  default:
    break;
  }
}

// Initialize MQTT Client
auto mqtt_app_start(ServerConfig const &config) -> void {
  char url[256];
  std::snprintf(url, sizeof(url), "mqtt://%d.%d.%d.%d:%d", config.ip_address[0],
                config.ip_address[1], config.ip_address[2],
                config.ip_address[3], config.port);

  esp_mqtt_client_config_t mqtt_cfg = {};
  mqtt_cfg.broker.address.uri = url;

  mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(
      mqtt_client, static_cast<esp_mqtt_event_id_t>(ESP_EVENT_ANY_ID),
      mqtt_event_handler, nullptr);
  esp_mqtt_client_start(mqtt_client);
}

// Task: Read Sensor & Publish Loop
auto temp_publish_task(void *pvParameters) -> void {
  if (mqtt_client == nullptr) {
    ESP_LOGW(TAG, "MQTT client not initialized; skipping publish");
    vTaskDelete(nullptr);
    return;
  }

  while (true) {
    // Read hardware temperature
    auto const tsens_out = iface.get_temperature_celsius();
    char temp_str[256];
    std::snprintf(temp_str, sizeof(temp_str), "\"temperature\": %d",
                  static_cast<int>(tsens_out));

    // Publish to topic: sensors/esp32c6/temperature
    int const msg_id = esp_mqtt_client_publish(
        mqtt_client, "sensors/esp32c6/temperature", temp_str, 0, 1, 0);

    ESP_LOGI(TAG, "published temp %.2f °C, msg_id=%d", tsens_out, msg_id);

    auto const battery_voltage = iface.get_battery_voltage_mV();
    char battery_str[256];
    std::snprintf(battery_str, sizeof(battery_str), "\"battery_voltage\": %ld",
                  battery_voltage);
    int const battery_msg_id = esp_mqtt_client_publish(
        mqtt_client, "sensors/esp32c6/battery_voltage", battery_str, 0, 1, 0);

    ESP_LOGI(TAG, "published battery voltage %.2f V, msg_id=%d",
             battery_voltage, battery_msg_id);

    freertos::delay(5s);
  }
}

auto nvs_init() -> void {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
}

} // namespace

auto start(ServerConfig const &config, WifiCredentials const &credentials,
           Iface _iface) -> void {
  nvs_init();

  iface = _iface;

  // Replace these placeholders with credentials from your provisioning source.
  if (wifi_init_sta(credentials) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize Wi-Fi with provided credentials");
    return;
  }

  mqtt_app_start(config);

  // 4. Start Publish Loop
  xTaskCreate(temp_publish_task, "temp_publish_task", 4096, nullptr, 5,
              nullptr);
}

} // namespace mqtt_server