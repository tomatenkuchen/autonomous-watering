/**
 * @file server.hpp
 * @brief MQTT Server implementation for handling network communication
 *
 * This file contains the declarations for the MQTT server functionality,
 * including configuration structures, Wi-Fi credentials, and the interface for
 * retrieving sensor data. It provides the necessary components to initialize
 * and start the MQTT server, handle Wi-Fi connections, and manage sensor data
 * publishing.
 */

#pragma once

#include "credentials.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace mqtt_server {

struct Iface {
  std::function<float()> get_temperature_celsius = [] { return -273.2f; };
  std::function<std::int32_t()> get_battery_voltage_mV = [] { return 0; };
};

auto start(credentials::Server const &config,
           credentials::Wifi const &credentials, Iface _iface = Iface{})
    -> void;

} // namespace mqtt_server