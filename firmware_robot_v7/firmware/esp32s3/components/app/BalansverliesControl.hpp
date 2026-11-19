/**
 * @file BalansverliesControl.hpp
 * @brief Activity diagram BalansverliesControl::verwerkMeting (UC-S3-03).
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
 * @brief Stopt de motoren na een val en wacht tot de robot weer rechtop en stil staat.
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
  /// @brief Maakt de control met de objecten die hij gebruikt.
  BalansverliesControl(MotorDriver& motorDriver, PidRegelaar& pidRegelaar,
                       Sessie& sessie, VerstuurControl& verstuurControl,
                       BalansFilter& balansFilter);

  /// @brief Stopt de motoren, verhoogt de valteller en stuurt meteen een bericht.
  void start();

  /**
   * @brief Wordt elke REGELPERIODE aangeroepen.
   * @return KLAAR als hij 1 s rechtop en stil staat
   */
  Resultaat verwerkMeting(Meting meting);
};
