/**
 * @file BalansData.hpp
 * @brief Entity met de laatste balanswaarde en bewegingsrichting.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <cstdint>

/**
 * @class BalansData
 * @brief Laatste balanswaarde en richting, voor UC-S3-04 Versturen.
 *
 * BalansControl en FoutControl schrijven (taak RobotControl, core 1),
 * VerstuurControl leest (core 0). De mutex zorgt dat balanswaarde en
 * richting altijd als paar worden gelezen.
 */
class BalansData {
private:
  float balanswaarde = 0.0f;
  int8_t richting = 0;
  bool geldig = false;
  SemaphoreHandle_t mutex = nullptr;

public:
  BalansData();

  /**
   * @brief Schrijft nieuwe balansgegevens en markeert ze als geldig.
   * @param balanswaarde afwijking t.o.v. het nulpunt in graden
   * @param richting draairichting van de wielen: -1, 0 of 1
   */
  void schrijf(float balanswaarde, int8_t richting);

  /**
   * @brief Leest de balansgegevens.
   * @param balanswaarde hierin komt de balanswaarde
   * @param richting hierin komt de richting
   * @return true als de gegevens geldig zijn
   */
  bool lees(float& balanswaarde, int8_t& richting);

  /**
   * @brief Markeert de gegevens als ongeldig (bij een fout).
   */
  void markeerOngeldig();
};
