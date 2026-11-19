/**
 * @file BalansData.hpp
 * @brief Laatste balanswaarde en richting, gedeeld tussen RobotControl en VerstuurControl.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <cstdint>

/// @brief Beschermd met een mutex omdat twee taken erbij kunnen.
class BalansData {
private:
  float balanswaarde = 0.0f;
  int8_t richting = 0;
  bool geldig = false;
  SemaphoreHandle_t mutex = nullptr;

public:
  BalansData();

  void schrijf(float balanswaarde, int8_t richting);

  bool lees(float& balanswaarde, int8_t& richting);

  void markeerOngeldig();
};
