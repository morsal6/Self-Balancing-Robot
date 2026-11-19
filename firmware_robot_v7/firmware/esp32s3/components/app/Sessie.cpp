/**
 * @file Sessie.cpp
 * @brief Implementatie van Sessie.
 */
#include "Sessie.hpp"

#include "esp_random.h"

Sessie::Sessie() {
  mutex = xSemaphoreCreateMutex();
  configASSERT(mutex != NULL);
}

void Sessie::nieuweSessie() {
  uint32_t nieuwId = esp_random();
  if (nieuwId == 0) {
    nieuwId = 1;
  }
  xSemaphoreTake(mutex, portMAX_DELAY);
  sessieId = nieuwId;
  valteller = 0;
  xSemaphoreGive(mutex);
}

void Sessie::verhoogValteller() {
  xSemaphoreTake(mutex, portMAX_DELAY);
  valteller++;
  xSemaphoreGive(mutex);
}

uint32_t Sessie::getSessieId() {
  xSemaphoreTake(mutex, portMAX_DELAY);
  const uint32_t id = sessieId;
  xSemaphoreGive(mutex);
  return id;
}

uint32_t Sessie::getValteller() {
  xSemaphoreTake(mutex, portMAX_DELAY);
  const uint32_t teller = valteller;
  xSemaphoreGive(mutex);
  return teller;
}
