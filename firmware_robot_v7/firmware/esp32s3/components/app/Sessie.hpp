/**
 * @file Sessie.hpp
 * @brief Sessie-id en valteller voor de score op de CYD.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <cstdint>

/**
 * @class Sessie
 * @brief Elke opstart krijgt een nieuw willekeurig sessie-id, zodat de CYD een herstart herkent.
 */
class Sessie {
private:
  uint32_t sessieId = 0;
  uint32_t valteller = 0;
  SemaphoreHandle_t mutex = nullptr;

public:
  /// @brief Maakt de mutex aan.
  Sessie();

  /// @brief Nieuw willekeurig sessie-id en valteller op 0.
  void nieuweSessie();

  /// @brief Telt een val.
  void verhoogValteller();

  /// @return het sessie-id
  uint32_t getSessieId();

  /// @return aantal vallen in deze sessie
  uint32_t getValteller();
};
