/**
 * @file BalansverliesControl.hpp
 * @brief Control-object van UC-S3-03 Balansverlies afhandelen.
 */
#pragma once

#include "BalansFilter.hpp"
#include "Meting.hpp"
#include "MotorDriver.hpp"
#include "PidRegelaar.hpp"
#include "Resultaat.hpp"
#include "Sessie.hpp"
#include "VerstuurControl.hpp"

#include <cstdint>

/**
 * @class BalansverliesControl
 * @brief "Dirigent" van UC-S3-03. Passief: wordt uitgevoerd door
 *        RobotControl, in de taak van RobotControl.
 *
 * Volgt het activity diagram BalansverliesControl::verwerkMeting(meting).
 */
class BalansverliesControl {
private:
  uint16_t hersteltijdTeller = 0;
  MotorDriver& motorDriver;
  PidRegelaar& pidRegelaar;
  Sessie& sessie;
  VerstuurControl& verstuurControl;
  BalansFilter& balansFilter;

public:
  BalansverliesControl(MotorDriver& motorDriver, PidRegelaar& pidRegelaar,
                       Sessie& sessie, VerstuurControl& verstuurControl,
                       BalansFilter& balansFilter);

  /**
   * @brief Handelt het balansverlies af: motoren stil, valteller +1.
   */
  void start();

  /**
   * @brief Verwerkt een meting: wacht tot de robot rechtop en stil staat.
   * @param meting de nieuwe meting
   * @return KLAAR als de robot weer mag balanceren, FOUT bij een ongeldige
   *         meting, anders BEZIG
   */
  Resultaat verwerkMeting(Meting meting);
};
