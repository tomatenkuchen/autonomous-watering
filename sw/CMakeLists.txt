##################################################################################################
# cmake boilerplate
cmake_minimum_required(VERSION 4.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "build type: Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "project build type" FORCE)
endif()

###################################################################################################
# dependencies

list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/generators")

find_package(etl REQUIRED)

set(LOCAL_CREDENTIALS_FILE "${CMAKE_SOURCE_DIR}/credentials.json")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${LOCAL_CREDENTIALS_FILE}")
if(NOT EXISTS "${LOCAL_CREDENTIALS_FILE}")
  message(FATAL_ERROR "Missing ${LOCAL_CREDENTIALS_FILE}; copy credentials.example.json and fill in local values")
endif()

file(READ "${LOCAL_CREDENTIALS_FILE}" LOCAL_CREDENTIALS_JSON)
string(JSON WIFI_SSID ERROR_VARIABLE WIFI_SSID_ERROR GET "${LOCAL_CREDENTIALS_JSON}" wifi_ssid)
string(JSON WIFI_PASSWORD ERROR_VARIABLE WIFI_PASSWORD_ERROR GET "${LOCAL_CREDENTIALS_JSON}" wifi_password)
string(JSON MQTT_PORT ERROR_VARIABLE MQTT_PORT_ERROR GET "${LOCAL_CREDENTIALS_JSON}" mqtt_broker_port)
string(JSON MQTT_IP_LENGTH ERROR_VARIABLE MQTT_IP_ERROR LENGTH "${LOCAL_CREDENTIALS_JSON}" mqtt_broker_ip)
if(WIFI_SSID_ERROR OR WIFI_PASSWORD_ERROR OR MQTT_PORT_ERROR OR MQTT_IP_ERROR OR
   NOT MQTT_IP_LENGTH EQUAL 4)
  message(FATAL_ERROR "credentials.json must define wifi_ssid, wifi_password, four mqtt_broker_ip octets, and mqtt_broker_port")
endif()

foreach(INDEX RANGE 0 3)
  string(JSON MQTT_IP_${INDEX} GET "${LOCAL_CREDENTIALS_JSON}" mqtt_broker_ip ${INDEX})
  if(NOT MQTT_IP_${INDEX} MATCHES "^[0-9]+$" OR MQTT_IP_${INDEX} GREATER 255)
    message(FATAL_ERROR "mqtt_broker_ip entries must be integers from 0 to 255")
  endif()
endforeach()
if(NOT MQTT_PORT MATCHES "^[0-9]+$" OR MQTT_PORT LESS 1 OR MQTT_PORT GREATER 65535)
  message(FATAL_ERROR "mqtt_broker_port must be an integer from 1 to 65535")
endif()

function(cpp_escape_string OUTPUT INPUT)
  set(VALUE "${INPUT}")
  string(REPLACE "\\" "\\\\" VALUE "${VALUE}")
  string(REPLACE "\"" "\\\"" VALUE "${VALUE}")
  string(REPLACE "\n" "\\n" VALUE "${VALUE}")
  string(REPLACE "\r" "\\r" VALUE "${VALUE}")
  string(REPLACE "\t" "\\t" VALUE "${VALUE}")
  set(${OUTPUT} "${VALUE}" PARENT_SCOPE)
endfunction()

cpp_escape_string(WIFI_SSID_CPP "${WIFI_SSID}")
cpp_escape_string(WIFI_PASSWORD_CPP "${WIFI_PASSWORD}")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
set(LOCAL_CREDENTIALS_HEADER "#pragma once\n\n#include <array>\n#include <cstdint>\n\nnamespace mqtt_server::local_config {\n\n")
string(APPEND LOCAL_CREDENTIALS_HEADER
  "inline constexpr std::array<std::uint8_t, 4> mqtt_broker_ip{${MQTT_IP_0}, ${MQTT_IP_1}, ${MQTT_IP_2}, ${MQTT_IP_3}};\n"
  "inline constexpr std::uint16_t mqtt_broker_port = ${MQTT_PORT};\n"
  "inline constexpr char wifi_ssid[] = \"${WIFI_SSID_CPP}\";\n"
  "inline constexpr char wifi_password[] = \"${WIFI_PASSWORD_CPP}\";\n\n"
  "} // namespace mqtt_server::local_config\n")
file(WRITE "${CMAKE_BINARY_DIR}/generated/local_credentials.hpp"
  "${LOCAL_CREDENTIALS_HEADER}")

###################################################################################################
# set up esp-idf environment
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
 
###################################################################################################
# project definition

project(${CONAN_PROJECT_NAME}
  DESCRIPTION ${CONAN_PROJECT_DESCRIPTION}
  VERSION ${CONAN_PROJECT_VERSION_MAJOR}.${CONAN_PROJECT_VERSION_MINOR}.${CONAN_PROJECT_VERSION_PATCH}
  LANGUAGES C CXX
)

configure_file(
   ${CMAKE_SOURCE_DIR}/cmake/version.hpp.in
   ${CMAKE_BINARY_DIR}/generated/version.hpp
)
