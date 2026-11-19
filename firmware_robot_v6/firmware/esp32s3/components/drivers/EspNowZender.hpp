/**
 * @file EspNowZender.hpp
 * @brief Verstuurt berichten naar de CYD via ESP-NOW.
 */
#pragma once

#include "Bericht.hpp"

class EspNowZender {
public:
  EspNowZender();

  /// @brief esp_now_init en de CYD als peer toevoegen.
  void configureer();

  void verstuur(Bericht bericht);
};
