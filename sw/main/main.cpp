/**
 * @file main.cpp
 * @brief entry point of executable
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "power.hpp"
#include "valves.hpp"

namespace
{

const static char *TAG = "main";

} // namespace

extern "C" void app_main()
{
    bsp::Power power;

    std::uint16_t v_cond_out_mV = 0;

    bsp::valves::Valves valves;

    while (true)
    {
        ESP_LOGI(TAG, "main loop");
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "USB voltage online: %s", power.is_usb_voltage_online() ? "yes" : "no");

        ESP_LOGI(TAG, "voltages: \n\tsolar: %d mV\n\tbattery: %dmV\n\n",
                 power.get_voltage_mV(bsp::Power::AdcInputs::v_solar),
                 power.get_voltage_mV(bsp::Power::AdcInputs::v_battery));

        ESP_LOGI(TAG, "currents: \n\tsolar: %d mA\n\tconductivity: %dmA\n\tusb: %d mA\n\n",
                 power.get_voltage_mV(bsp::Power::AdcInputs::i_solar),
                 power.get_voltage_mV(bsp::Power::AdcInputs::i_conductivity),
                 power.get_voltage_mV(bsp::Power::AdcInputs::i_usb));

        v_cond_out_mV += 1024;
        power.set_conductivity_voltage_mV(v_cond_out_mV);
    }
}
