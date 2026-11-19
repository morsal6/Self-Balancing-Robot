/**
 * @file BerichtListener.hpp
 * @brief Interface voor wie berichten van EspNowOntvanger wil krijgen.
 */
#pragma once

#include "Bericht.hpp"

class BerichtListener {
public:
  virtual void berichtOntvangen(Bericht bericht) = 0;

  virtual ~BerichtListener() = default;
};
