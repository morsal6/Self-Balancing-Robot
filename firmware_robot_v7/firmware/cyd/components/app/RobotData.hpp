/**
 * @file RobotData.hpp
 * @brief Laatst ontvangen bericht van de robot.
 */
#pragma once

#include "Bericht.hpp"

/**
 * @class RobotData
 * @brief Laatst ontvangen bericht en of het nog actueel is.
 */
class RobotData {
private:
  Bericht laatsteBericht = {};
  bool actueel = false;

public:
  /// @brief Slaat een geldig bericht op.
  void schrijf(Bericht bericht);

  /// @brief Te lang geen bericht gehad.
  void markeerVerouderd();
};
