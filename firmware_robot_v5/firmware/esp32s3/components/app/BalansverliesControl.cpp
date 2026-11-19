/**
 * @file BalansverliesControl.cpp
 * @brief Implementatie van het activity diagram BalansverliesControl::verwerkMeting.
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
  // d1
  if (!meting.geldig) {
    return Resultaat::FOUT;
  }
  const float balanswaarde = balansFilter.bereken(meting);

  // d2
  if (std::fabs(balanswaarde) >= HERSTARTGRENS || std::fabs(meting.gy) >= STILGRENS) {
    hersteltijdTeller = 0;
    return Resultaat::BEZIG;
  }
  hersteltijdTeller++;

  // d3
  if (hersteltijdTeller < HERSTEL_AANTAL) {
    return Resultaat::BEZIG;
  }
  pidRegelaar.init();
  return Resultaat::KLAAR;
}
