idf_component_register(
  SRCS "Mpu6050.cpp" "MotorDriver.cpp" "EspNowZender.cpp"
  INCLUDE_DIRS "."
  REQUIRES core protocol freertos esp_driver_gpio esp_driver_i2c esp_driver_ledc
           esp_wifi esp_netif esp_event nvs_flash log
)
