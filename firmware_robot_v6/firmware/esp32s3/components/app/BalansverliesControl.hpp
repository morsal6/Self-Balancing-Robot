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

/// @brief Stopt de motoren na een val en wacht tot de robot weer rechtop en stil staat.
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

  void start();

  Resultaat verwerkMeting(Meting meting);
};
