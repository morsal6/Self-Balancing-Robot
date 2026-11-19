/**
 * @file BalansFilter.cpp
 * @brief Implementatie van BalansFilter.
 */
#include "BalansFilter.hpp"

#include "Ontwerpparameters.hpp"

#include <cmath>

namespace {
constexpr float RAD_NAAR_GRADEN = 57.2957795f;
}

float BalansFilter::bereken(Meting meting) {
  // De MPU zit ondersteboven, daarom -ax.
  float accHoek = std::atan2(-meting.ax, meting.az) * RAD_NAAR_GRADEN;

  if (std::isnan(hoek)) {
    hoek = accHoek;
  }

  // Rechtop is ongeveer 180 graden. Zorg dat accHoek niet van -180 naar 180 springt.
  while (accHoek - hoek > 180.0f) accHoek -= 360.0f;
  while (accHoek - hoek < -180.0f) accHoek += 360.0f;

  hoek = FILTER_ALFA * (hoek + meting.gy * REGELPERIODE_S) + (1.0f - FILTER_ALFA) * accHoek;
  return hoek - nulpunt;
}

void BalansFilter::bepaalNulpunt() {
  nulpunt = hoek;
}
