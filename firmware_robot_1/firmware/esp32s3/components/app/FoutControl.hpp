/**
 * @file FoutControl.hpp
 * @brief Control-object van UC-S3-05 Fout afhandelen.
 */
#pragma once

#include "BalansData.hpp"
#include "MotorDriver.hpp"

/**
 * @class FoutControl
 * @brief "Dirigent" van UC-S3-05. Passief: wordt uitgevoerd door RobotControl.
 */
class FoutControl {
private:
  MotorDriver& motorDriver;
  BalansData& balansData;

public:
  FoutControl(MotorDriver& motorDriver, BalansData& balansData);

  /**
   * @brief Zet de motoren stil en markeert de balansgegevens als ongeldig.
   */
  void handelAf();
};
