/**
 * @file BalansControl.cpp
 * @brief Implementatie van het activity diagram BalansControl::verwerkMeting.
 */
#include "BalansControl.hpp"

#include "Ontwerpparameters.hpp"

#include <cmath>

namespace {
/// Teken van een getal: -1, 0 of 1 (de bewegingsrichting).
int8_t sign(int16_t waarde) {
  return static_cast<int8_t>((waarde > 0) - (waarde < 0));
}
}  // namespace

BalansControl::BalansControl(BalansFilter& balansFilter, PidRegelaar& pidRegelaar,
                             MotorDriver& motorDriver, BalansData& balansData)
    : balansFilter(balansFilter), pidRegelaar(pidRegelaar),
      motorDriver(motorDriver), balansData(balansData) {}

void BalansControl::start() {
  buitenGrensTeller = 0;
}

Resultaat BalansControl::verwerkMeting(Meting meting) {
  // d1
  if (!meting.geldig) {
    motorDriver.stuur(0, 0);
    return Resultaat::FOUT;
  }
  const float balanswaarde = balansFilter.bereken(meting);

  // d2
  if (std::fabs(balanswaarde) > BALANSVERLIESGRENS) {
    buitenGrensTeller++;
    // d3
    if (buitenGrensTeller >= BEVESTIGINGSAANTAL) {
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
