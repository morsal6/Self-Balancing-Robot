/**
 * @file EspNowOntvanger.hpp
 * @brief Proxy-object van de ESP32-S3: ontvangt berichten via ESP-NOW.
 */
#pragma once

#include "Bericht.hpp"
#include "BerichtListener.hpp"

/**
 * @class EspNowOntvanger
 * @brief Ontvangt berichten via ESP-NOW en geeft ze door aan zijn listener.
 *
 * De ontvangst-callback draait in de WiFi-taak van ESP-IDF (core 0). Daar
 * wordt alleen het formaat gecontroleerd; de listener zet het bericht in zijn
 * eigen queue.
 */
class EspNowOntvanger {
private:
  BerichtListener* listener = nullptr;

public:
  /**
   * @brief Start WiFi en ESP-NOW op ESPNOW_KANAAL en begint met ontvangen.
   */
  void init();

  /**
   * @brief Meldt de listener aan. Moet voor init() gebeuren.
   * @param listener de ontvanger van de berichten
   */
  void addListener(BerichtListener* listener);
};
