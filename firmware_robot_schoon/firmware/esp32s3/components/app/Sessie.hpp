/**
 * @file Sessie.hpp
 * @brief Entity met het sessie-ID en de valteller.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <cstdint>

/**
 * @class Sessie
 * @brief Bewaart het sessie-ID van deze start en het aantal balansverliezen.
 *
 * Wordt geschreven vanuit de taak van RobotControl en gelezen door
 * VerstuurControl, daarom beschermd met een mutex.
 */
class Sessie {
private:
  uint32_t sessieId = 0;
  uint32_t valteller = 0;
  SemaphoreHandle_t mutex = nullptr;

public:
  Sessie();

  /**
   * @brief Start een nieuwe sessie: nieuw willekeurig sessie-ID, valteller 0.
   */
  void nieuweSessie();

  /**
   * @brief Verhoogt de valteller met precies één (FR-04).
   */
  void verhoogValteller();

  /**
   * @brief Geeft het sessie-ID.
   * @return het sessie-ID
   */
  uint32_t getSessieId();

  /**
   * @brief Geeft de valteller.
   * @return het aantal balansverliezen in deze sessie
   */
  uint32_t getValteller();
};
