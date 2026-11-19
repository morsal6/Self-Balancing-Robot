/**
 * @file BalansFilter.hpp
 * @brief Complementair filter dat metingen omzet naar een balanswaarde.
 */
#pragma once

#include "Meting.hpp"

#include <cmath>

/**
 * @class BalansFilter
 * @brief Combineert accelerometer en gyroscoop tot een kantelhoek.
 *
 * De robot kantelt om de y-as van de MPU6050 (gemeten). Voorover geeft een
 * positieve balanswaarde, achterover een negatieve.
 */
class BalansFilter {
private:
  float hoek = NAN;      ///< Gefilterde kantelhoek in graden; NAN tot de eerste meting
  float nulpunt = 0.0f;  ///< Kantelhoek waarbij de robot rechtop staat

public:
  /**
   * @brief Verwerkt een meting.
   * @param meting een geldige meting
   * @return balanswaarde: de kantelhoek min het nulpunt, in graden
   */
  float bereken(Meting meting);

  /**
   * @brief Legt de huidige kantelhoek vast als nulpunt.
   */
  void bepaalNulpunt();
};
