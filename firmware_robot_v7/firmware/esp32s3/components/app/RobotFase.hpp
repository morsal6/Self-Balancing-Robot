/**
 * @file RobotFase.hpp
 * @brief Fase van de robot die naar de CYD gestuurd wordt.
 */
#pragma once

#include "Bericht.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @class RobotFase
 * @brief Huidige fase, beschermd met een mutex.
 */
class RobotFase {
private:
  Fase fase = Fase::KALIBREREN;
  SemaphoreHandle_t mutex = nullptr;

public:
  /// @brief Maakt de mutex aan.
  RobotFase();

  /// @brief Zet de fase.
  void setFase(Fase fase);

  /// @return de huidige fase
  Fase getFase();
};
