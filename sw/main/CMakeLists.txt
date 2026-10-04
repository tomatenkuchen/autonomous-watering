idf_component_register(
  SRCS 
    "main.cpp" 
    "pwm.cpp" 
    "power.cpp" 
    "server.cpp"

  PRIV_REQUIRES 
    esp_adc 
    esp_driver_gpio 
    esp_driver_ledc 
    esp_driver_tsens
    esp_event
    esp_netif
    esp_pm
    esp_wifi
    mqtt 
    nvs_flash 

  INCLUDE_DIRS 
    "."
    "${CMAKE_BINARY_DIR}/generated"
)

target_link_libraries(${COMPONENT_LIB}
  PRIVATE
    etl::etl
)
