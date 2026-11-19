/**
 * @file BalansControl.cpp
 * @brief Implementatie van BalansControl.
 */
#include "BalansControl.hpp"

#include "Ontwerpparameters.hpp"

#include <cmath>

namespace {
int8_t sign(int16_t waarde) {
  return static_cast<int8_t>((waarde > 0) - (waarde < 0));
}
}

BalansControl::BalansControl(BalansFilter& balansFilter, PidRegelaar& pidRegelaar,
                             MotorDriver& motorDriver, BalansData& balansData)
    : balansFilter(balansFilter), pidRegelaar(pidRegelaar),
      motorDriver(motorDriver), balansData(balansData) {}

void BalansControl::start() {
  buitenGrensTeller = 0;
}

Resultaat BalansControl::verwerkMeting(Meting meting) {
  if (!meting.geldig) {  // d1
    motorDriver.stuur(0, 0);
    return Resultaat::FOUT;
  }
  const float balanswaarde = balansFilter.bereken(meting);

  if (std::fabs(balanswaarde) > BALANSVERLIESGRENS) {  // d2
    buitenGrensTeller++;

    if (buitenGrensTeller >= BEVESTIGINGSAANTAL) {  // d3
      return Resultaat::BALANSVERLIES;
    }
    return Resultaat::BEZIG;
  }
  buitenGrensTeller = 0;
  const int16_t correctie = pidRegelaar.bereken(balanswaarde);
  motorDriver.stuur(correctie, correctie);
  balansData.schrijf(balanswaarde, sign(correctie));
  return Resultaat::BEZIG;
}
