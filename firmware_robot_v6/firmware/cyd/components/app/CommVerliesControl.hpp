/**
 * @file CommVerliesControl.hpp
 * @brief Wat er gebeurt als er te lang geen bericht komt (UC-CYD-03).
 */
#pragma once

#include "Display.hpp"
#include "RobotData.hpp"

class CommVerliesControl {
private:
  RobotData& robotData;
  Display& display;

public:
  CommVerliesControl(RobotData& robotData, Display& display);

  void voerUit();
};
