/**
 * @file InitControl.cpp
 * @brief Implementatie van het activity diagram InitControl::verwerkMeting.
 */
#include "InitControl.hpp"

#include "Ontwerpparameters.hpp"

#include <cmath>

InitControl::InitControl(Sessie& sessie, MotorDriver& motorDriver, Mpu6050& mpu6050,
                         EspNowZender& espNowZender, BalansFilter& balansFilter,
                         PidRegelaar& pidRegelaar)
    : sessie(sessie), motorDriver(motorDriver), mpu6050(mpu6050),
      espNowZender(espNowZender), balansFilter(balansFilter), pidRegelaar(pidRegelaar) {}

bool InitControl::start() {
  sessie.nieuweSessie();
  const bool motorOk = motorDriver.stuur(0, 0);
  mpu6050.start();
  espNowZender.configureer();
  stilTeller = 0;
  return motorOk;
}

Resultaat InitControl::verwerkMeting(Meting meting) {
  // d1
  if (!meting.geldig) {
    return Resultaat::FOUT;
  }
  // d2
  if (std::fabs(meting.gy) > STILGRENS) {
    stilTeller = 0;
    return Resultaat::BEZIG;
  }
  stilTeller++;
  balansFilter.bereken(meting);

  // d3
  if (stilTeller < STIL_AANTAL) {
    return Resultaat::BEZIG;
  }
  balansFilter.bepaalNulpunt();
  pidRegelaar.init();
  return Resultaat::KLAAR;
}
