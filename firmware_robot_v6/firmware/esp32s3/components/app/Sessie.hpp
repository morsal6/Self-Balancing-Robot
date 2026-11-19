/**
 * @file Sessie.hpp
 * @brief Sessie-id en valteller voor de score op de CYD.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <cstdint>

/// @brief Elke opstart krijgt een nieuw willekeurig sessie-id, zodat de CYD een herstart herkent.
class Sessie {
private:
  uint32_t sessieId = 0;
  uint32_t valteller = 0;
  SemaphoreHandle_t mutex = nullptr;

public:
  Sessie();

  void nieuweSessie();

  void verhoogValteller();

  uint32_t getSessieId();

  uint32_t getValteller();
};
