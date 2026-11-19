/**
 * @file EspNowZender.hpp
 * @brief Verstuurt berichten naar de CYD via ESP-NOW.
 */
#pragma once

#include "Bericht.hpp"

/**
 * @class EspNowZender
 * @brief Verstuurt berichten naar de CYD via ESP-NOW.
 */
class EspNowZender {
public:
  /// @brief Start NVS en WiFi (station, vast kanaal).
  EspNowZender();

  /// @brief esp_now_init en de CYD als peer toevoegen.
  void configureer();

  /// @brief Verstuurt een bericht naar de CYD.
  void verstuur(Bericht bericht);
};
