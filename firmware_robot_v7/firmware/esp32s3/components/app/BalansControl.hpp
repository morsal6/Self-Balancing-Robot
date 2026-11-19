/**
 * @file BalansControl.hpp
 * @brief Activity diagram BalansControl::verwerkMeting (UC-S3-02).
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
 * @brief Houdt de robot in balans met de PID-regelaar en ziet of hij omvalt.
 */
class BalansControl {
private:
  uint16_t buitenGrensTeller = 0;
  BalansFilter& balansFilter;
  PidRegelaar& pidRegelaar;
  MotorDriver& motorDriver;
  BalansData& balansData;

public:
  /// @brief Maakt de control met de objecten die hij gebruikt.
  BalansControl(BalansFilter& balansFilter, PidRegelaar& pidRegelaar,
                MotorDriver& motorDriver, BalansData& balansData);

  /// @brief Zet de teller voor balansverlies op 0.
  void start();

  /// @brief Wordt elke REGELPERIODE door RobotControl aangeroepen.
  Resultaat verwerkMeting(Meting meting);
};
