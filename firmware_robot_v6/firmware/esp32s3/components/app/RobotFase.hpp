/**
 * @file RobotFase.hpp
 * @brief Fase van de robot die naar de CYD gestuurd wordt.
 */
#pragma once

#include "Bericht.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class RobotFase {
private:
  Fase fase = Fase::KALIBREREN;
  SemaphoreHandle_t mutex = nullptr;

public:
  RobotFase();

  void setFase(Fase fase);

  Fase getFase();
};
