/**
 * @file EspNowZender.hpp
 * @brief Proxy-object van de CYD: verstuurt berichten via ESP-NOW.
 */
#pragma once

#include "Bericht.hpp"

/**
 * @class EspNowZender
 * @brief Verstuurt een Bericht via ESP-NOW naar de CYD (naar CYD_MAC).
 *
 * De constructor start de WiFi-driver. Omdat de constructor in app_main
 * draait (core 0), gebeurt dat op dezelfde core als de WiFi-taak, weg van de
 * regellus op core 1. configureer() zet daarna ESP-NOW zelf aan.
 */
class EspNowZender {
public:
  /**
   * @brief Initialiseert NVS en start WiFi in station-modus op ESPNOW_KANAAL.
   */
  EspNowZender();

  /**
   * @brief Zet ESP-NOW aan en voegt het broadcastadres toe als peer.
   */
  void configureer();

  /**
   * @brief Verstuurt een bericht. Mislukt het, dan vervalt het bericht (UC-S3-04).
   * @param bericht het te versturen bericht
   */
  void verstuur(Bericht bericht);
};
