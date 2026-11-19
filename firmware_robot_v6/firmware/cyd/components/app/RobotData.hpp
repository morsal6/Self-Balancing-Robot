/**
 * @file RobotData.hpp
 * @brief Laatst ontvangen bericht van de robot.
 */
#pragma once

#include "Bericht.hpp"

class RobotData {
private:
  Bericht laatsteBericht = {};
  bool actueel = false;

public:
  void schrijf(Bericht bericht);

  void markeerVerouderd();
};
