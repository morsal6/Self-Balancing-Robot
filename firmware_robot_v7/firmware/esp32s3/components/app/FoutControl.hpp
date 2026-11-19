/**
 * @file FoutControl.hpp
 * @brief Afhandeling van een fout (UC-S3-05).
 */
#pragma once

#include "BalansData.hpp"
#include "MotorDriver.hpp"

/**
 * @class FoutControl
 * @brief Zet de motoren uit en markeert de balansdata als ongeldig.
 */
class FoutControl {
private:
  MotorDriver& motorDriver;
  BalansData& balansData;

public:
  /// @brief Maakt de control met de objecten die hij gebruikt.
  FoutControl(MotorDriver& motorDriver, BalansData& balansData);

  /// @brief Motoren uit en balansdata ongeldig.
  void handelAf();
};
