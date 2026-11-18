/**
 * @file RobotFase.hpp
 * @brief Entity met de huidige fase van de robot.
 */
#pragma once

#include "Bericht.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @class RobotFase
 * @brief Bewaart de fase. RobotControl schrijft, VerstuurControl leest.
 *
 * Omdat twee taken (op twee cores) de fase gebruiken, is de toegang
 * beschermd met een mutex.
 */
class RobotFase {
private:
  Fase fase = Fase::KALIBREREN;
  SemaphoreHandle_t mutex = nullptr;

public:
  RobotFase();

  /**
   * @brief Zet de fase.
   * @param fase de nieuwe fase
   */
  void setFase(Fase fase);

  /**
   * @brief Geeft de fase.
   * @return de huidige fase
   */
  Fase getFase();
};
