/**
 * @file BalansData.hpp
 * @brief Laatste balanswaarde en richting, gedeeld tussen RobotControl en VerstuurControl.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <cstdint>

/**
 * @class BalansData
 * @brief Beschermd met een mutex omdat twee taken erbij kunnen.
 */
class BalansData {
private:
  float balanswaarde = 0.0f;
  int8_t richting = 0;
  bool geldig = false;
  SemaphoreHandle_t mutex = nullptr;

public:
  /// @brief Maakt de mutex aan.
  BalansData();

  /**
   * @brief Slaat de laatste balanswaarde op.
   * @param balanswaarde kanteling in graden
   * @param richting -1 achteruit, 0 stil, 1 vooruit
   */
  void schrijf(float balanswaarde, int8_t richting);

  /**
   * @brief Leest de laatste waarden.
   * @return false als de data ongeldig is (na een fout)
   */
  bool lees(float& balanswaarde, int8_t& richting);

  /// @brief Markeert de data als ongeldig.
  void markeerOngeldig();
};
