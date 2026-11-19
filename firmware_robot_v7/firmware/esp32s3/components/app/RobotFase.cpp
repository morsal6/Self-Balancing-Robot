/**
 * @file RobotFase.cpp
 * @brief Implementatie van RobotFase.
 */
#include "RobotFase.hpp"

RobotFase::RobotFase() {
  mutex = xSemaphoreCreateMutex();
  configASSERT(mutex != NULL);
}

void RobotFase::setFase(Fase fase) {
  xSemaphoreTake(mutex, portMAX_DELAY);
  this->fase = fase;
  xSemaphoreGive(mutex);
}

Fase RobotFase::getFase() {
  xSemaphoreTake(mutex, portMAX_DELAY);
  const Fase huidig = fase;
  xSemaphoreGive(mutex);
  return huidig;
}
