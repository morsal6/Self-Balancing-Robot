/**
 * @file RobotData.cpp
 * @brief Implementatie van RobotData.
 */
#include "RobotData.hpp"

void RobotData::schrijf(Bericht bericht) {
  laatsteBericht = bericht;
  actueel = true;
}

void RobotData::markeerVerouderd() {
  actueel = false;
}
