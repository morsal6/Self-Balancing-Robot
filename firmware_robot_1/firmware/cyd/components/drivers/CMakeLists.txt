idf_component_register(
  SRCS "EspNowOntvanger.cpp" "Touchscreen.cpp" "Display.cpp"
  INCLUDE_DIRS "."
  REQUIRES core protocol ili9341_display drawables freertos esp_driver_gpio
           esp_wifi esp_netif heap esp_event nvs_flash log
)
