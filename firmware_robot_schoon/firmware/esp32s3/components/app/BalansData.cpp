/**
 * @file BalansData.cpp
 * @brief Implementatie van BalansData.
 */
#include "BalansData.hpp"

BalansData::BalansData() {
  mutex = xSemaphoreCreateMutex();
  configASSERT(mutex != NULL);
}

void BalansData::schrijf(float balanswaarde, int8_t richting) {
  xSemaphoreTake(mutex, portMAX_DELAY);
  this->balanswaarde = balanswaarde;
  this->richting = richting;
  geldig = true;
  xSemaphoreGive(mutex);
}

bool BalansData::lees(float& balanswaarde, int8_t& richting) {
  xSemaphoreTake(mutex, portMAX_DELAY);
  balanswaarde = this->balanswaarde;
  richting = this->richting;
  const bool isGeldig = geldig;
  xSemaphoreGive(mutex);
  return isGeldig;
}

void BalansData::markeerOngeldig() {
  xSemaphoreTake(mutex, portMAX_DELAY);
  geldig = false;
  xSemaphoreGive(mutex);
}
