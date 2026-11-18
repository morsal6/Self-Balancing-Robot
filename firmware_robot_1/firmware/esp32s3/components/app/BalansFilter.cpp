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
  // Hoek volgens de zwaartekracht (traag maar zonder drift) ...
  float accHoek = std::atan2(-meting.ax, meting.az) * RAD_NAAR_GRADEN;
  // atan2 springt bij +-180 graden. Houd accHoek binnen 180 graden van de
  // huidige hoek, zodat het filter niet springt als de sensor in de
  // rechtopstand ondersteboven zit (az ongeveer -1 g).
  while (accHoek - hoek > 180.0f) accHoek -= 360.0f;
  while (accHoek - hoek < -180.0f) accHoek += 360.0f;
  // ... gecombineerd met de geïntegreerde hoeksnelheid (snel maar met drift).
  hoek = FILTER_ALFA * (hoek + meting.gy * REGELPERIODE_S) + (1.0f - FILTER_ALFA) * accHoek;
  return hoek - nulpunt;
}

void BalansFilter::bepaalNulpunt() {
  nulpunt = hoek;
}
