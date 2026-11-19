/**
 * @file BalansFilter.hpp
 * @brief Complementair filter dat de kanteling berekent.
 */
#pragma once

#include "Meting.hpp"

#include <cmath>

/// @brief Combineert gyro en accelerometer tot een hoek ten opzichte van het nulpunt.
class BalansFilter {
private:
  float hoek = NAN;
  float nulpunt = 0.0f;

public:
  /// @brief Geeft de balanswaarde in graden (voorover is positief).
  float bereken(Meting meting);

  /// @brief Huidige hoek wordt het nulpunt (robot staat dan rechtop).
  void bepaalNulpunt();
};
