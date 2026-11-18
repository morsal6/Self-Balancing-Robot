idf_component_register(
  SRCS "TonenControl.cpp" "ScoreControl.cpp" "CommVerliesControl.cpp" "RobotData.cpp"
  INCLUDE_DIRS "."
  REQUIRES core protocol drivers freertos esp_driver_gpio log
)
