idf_component_register(
  SRCS "main.cpp" "pwm.cpp"
  PRIV_REQUIRES esp_adc esp_driver_ledc esp_driver_gpio esp_pm
  INCLUDE_DIRS "."
)

target_link_libraries(${COMPONENT_LIB}
  PRIVATE
    etl::etl
)
