/**
 * @file InitControl.cpp
 * @brief Implementatie van InitControl.
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
  if (!meting.geldig) {  // d1
    return Resultaat::FOUT;
  }

  if (std::fabs(meting.gy) > STILGRENS) {  // d2: beweegt nog
    stilTeller = 0;
    return Resultaat::BEZIG;
  }
  stilTeller++;
  balansFilter.bereken(meting);

  if (stilTeller < STIL_AANTAL) {  // d3
    return Resultaat::BEZIG;
  }
  balansFilter.bepaalNulpunt();
  pidRegelaar.init();
  return Resultaat::KLAAR;
}
