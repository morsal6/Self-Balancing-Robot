/**
 * @file InitControl.hpp
 * @brief Activity diagram InitControl::verwerkMeting (UC-S3-01).
 */
#pragma once

#include "BalansFilter.hpp"
#include "EspNowZender.hpp"
#include "Meting.hpp"
#include "MotorDriver.hpp"
#include "Mpu6050.hpp"
#include "PidRegelaar.hpp"
#include "Resultaat.hpp"
#include "Sessie.hpp"

#include <cstdint>

/**
 * @class InitControl
 * @brief Start alles op en bepaalt het nulpunt als de robot 2 s stil staat.
 */
class InitControl {
private:
  uint16_t stilTeller = 0;
  Sessie& sessie;
  MotorDriver& motorDriver;
  Mpu6050& mpu6050;
  EspNowZender& espNowZender;
  BalansFilter& balansFilter;
  PidRegelaar& pidRegelaar;

public:
  /// @brief Maakt de control met de objecten die hij gebruikt.
  InitControl(Sessie& sessie, MotorDriver& motorDriver, Mpu6050& mpu6050,
              EspNowZender& espNowZender, BalansFilter& balansFilter,
              PidRegelaar& pidRegelaar);

  /// @brief Geeft false als de motoren niet aangestuurd kunnen worden.
  bool start();

  /**
   * @brief Wordt elke REGELPERIODE aangeroepen.
   * @return KLAAR als het nulpunt bepaald is
   */
  Resultaat verwerkMeting(Meting meting);
};
