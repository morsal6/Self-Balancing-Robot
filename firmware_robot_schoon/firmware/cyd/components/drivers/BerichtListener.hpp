/**
 * @file BerichtListener.hpp
 * @brief Interface voor objecten die berichten van de EspNowOntvanger ontvangen.
 */
#pragma once

#include "Bericht.hpp"

/**
 * @class BerichtListener
 * @brief Listener-interface van de EspNowOntvanger.
 */
class BerichtListener {
public:
  /**
   * @brief Wordt aangeroepen als er een bericht van de ESP32-S3 binnenkomt.
   * @param bericht het ontvangen bericht
   */
  virtual void berichtOntvangen(Bericht bericht) = 0;

  virtual ~BerichtListener() = default;
};
