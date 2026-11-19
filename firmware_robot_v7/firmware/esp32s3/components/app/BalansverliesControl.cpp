/**
 * @file BalansverliesControl.cpp
 * @brief Implementatie van BalansverliesControl.
 */
#include "BalansverliesControl.hpp"

#include "Ontwerpparameters.hpp"

#include <cmath>

BalansverliesControl::BalansverliesControl(MotorDriver& motorDriver, PidRegelaar& pidRegelaar,
                                           Sessie& sessie, VerstuurControl& verstuurControl,
                                           BalansFilter& balansFilter)
    : motorDriver(motorDriver), pidRegelaar(pidRegelaar), sessie(sessie),
      verstuurControl(verstuurControl), balansFilter(balansFilter) {}

void BalansverliesControl::start() {
  motorDriver.stuur(0, 0);
  pidRegelaar.resetIntegraal();
  sessie.verhoogValteller();
  verstuurControl.verstuur();
  hersteltijdTeller = 0;
}

Resultaat BalansverliesControl::verwerkMeting(Meting meting) {
  if (!meting.geldig) {  // d1
    return Resultaat::FOUT;
  }
  const float balanswaarde = balansFilter.bereken(meting);

  if (std::fabs(balanswaarde) >= HERSTARTGRENS || std::fabs(meting.gy) >= STILGRENS) {  // d2: nog niet rechtop of stil
    hersteltijdTeller = 0;
    return Resultaat::BEZIG;
  }
  hersteltijdTeller++;

  if (hersteltijdTeller < HERSTEL_AANTAL) {  // d3
    return Resultaat::BEZIG;
  }
  pidRegelaar.init();
  return Resultaat::KLAAR;
}
