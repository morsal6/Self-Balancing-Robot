/**
 * @file FoutControl.hpp
 * @brief Afhandeling van een fout (UC-S3-05).
 */
#pragma once

#include "BalansData.hpp"
#include "MotorDriver.hpp"

/// @brief Zet de motoren uit en markeert de balansdata als ongeldig.
class FoutControl {
private:
  MotorDriver& motorDriver;
  BalansData& balansData;

public:
  FoutControl(MotorDriver& motorDriver, BalansData& balansData);

  void handelAf();
};
