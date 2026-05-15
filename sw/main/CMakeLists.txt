idf_component_register(
  SRCS "main.cpp"
  PRIV_REQUIRES esp_adc
  INCLUDE_DIRS "."
)

target_link_libraries(${COMPONENT_LIB}
  PRIVATE
    etl::etl
)
