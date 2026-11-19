/**
 * @file EspNowOntvanger.hpp
 * @brief Ontvangt berichten van de S3 via ESP-NOW.
 */
#pragma once

#include "Bericht.hpp"
#include "BerichtListener.hpp"

/**
 * @class EspNowOntvanger
 * @brief Ontvangt berichten van de S3 via ESP-NOW.
 */
class EspNowOntvanger {
private:
  BerichtListener* listener = nullptr;

public:
  /// @brief Start NVS, WiFi en ESP-NOW en registreert de ontvangst-callback.
  void init();

  /// @brief Zet wie de berichten krijgt.
  void addListener(BerichtListener* listener);
};
