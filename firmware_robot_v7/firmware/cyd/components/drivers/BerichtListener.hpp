/**
 * @file BerichtListener.hpp
 * @brief Interface voor wie berichten van EspNowOntvanger wil krijgen.
 */
#pragma once

#include "Bericht.hpp"

/**
 * @class BerichtListener
 * @brief Interface voor wie berichten van EspNowOntvanger wil krijgen.
 */
class BerichtListener {
public:
  /// @brief Wordt aangeroepen vanuit de WiFi-taak.
  virtual void berichtOntvangen(Bericht bericht) = 0;

  virtual ~BerichtListener() = default;
};
