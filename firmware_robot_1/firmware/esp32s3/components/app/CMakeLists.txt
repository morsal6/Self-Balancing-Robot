idf_component_register(
  SRCS "RobotControl.cpp" "InitControl.cpp" "BalansControl.cpp"
       "BalansverliesControl.cpp" "FoutControl.cpp" "VerstuurControl.cpp"
       "RobotFase.cpp" "Sessie.cpp" "BalansData.cpp" "BalansFilter.cpp"
       "PidRegelaar.cpp"
  INCLUDE_DIRS "."
  REQUIRES core protocol drivers freertos esp_hw_support log
)
