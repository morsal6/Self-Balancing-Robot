/**
 * @file InitControl.hpp
 * @brief Control-object van UC-S3-01 Initialiseren.
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
 * @brief "Dirigent" van UC-S3-01 Initialiseren. Passief: wordt uitgevoerd
 *        door RobotControl, in de taak van RobotControl.
 *
 * Volgt het activity diagram InitControl::verwerkMeting(meting).
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
  InitControl(Sessie& sessie, MotorDriver& motorDriver, Mpu6050& mpu6050,
              EspNowZender& espNowZender, BalansFilter& balansFilter,
              PidRegelaar& pidRegelaar);

  /**
   * @brief Start de initialisatie: nieuwe sessie, motoren stil, sensor en
   *        ESP-NOW aan.
   * @return false als de motoren niet stilgezet konden worden
   */
  bool start();

  /**
   * @brief Verwerkt een meting tijdens het kalibreren.
   * @param meting de nieuwe meting
   * @return KLAAR als het nulpunt bepaald is, FOUT bij een ongeldige meting,
   *         anders BEZIG
   */
  Resultaat verwerkMeting(Meting meting);
};
