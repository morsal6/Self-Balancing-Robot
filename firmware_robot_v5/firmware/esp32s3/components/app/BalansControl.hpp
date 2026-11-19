/**
 * @file BalansControl.hpp
 * @brief Control-object van UC-S3-02 Balanceren.
 */
#pragma once

#include "BalansData.hpp"
#include "BalansFilter.hpp"
#include "Meting.hpp"
#include "MotorDriver.hpp"
#include "PidRegelaar.hpp"
#include "Resultaat.hpp"

#include <cstdint>

/**
 * @class BalansControl
 * @brief "Dirigent" van UC-S3-02 Balanceren. Passief: wordt uitgevoerd door
 *        RobotControl, in de taak van RobotControl.
 *
 * Volgt het activity diagram BalansControl::verwerkMeting(meting).
 */
class BalansControl {
private:
  uint16_t buitenGrensTeller = 0;
  BalansFilter& balansFilter;
  PidRegelaar& pidRegelaar;
  MotorDriver& motorDriver;
  BalansData& balansData;

public:
  BalansControl(BalansFilter& balansFilter, PidRegelaar& pidRegelaar,
                MotorDriver& motorDriver, BalansData& balansData);

  /**
   * @brief Begint aan het balanceren.
   */
  void start();

  /**
   * @brief Verwerkt een meting: regelen of balansverlies vaststellen.
   * @param meting de nieuwe meting
   * @return BALANSVERLIES als de grens lang genoeg overschreden is, FOUT bij
   *         een ongeldige meting, anders BEZIG
   */
  Resultaat verwerkMeting(Meting meting);
};
