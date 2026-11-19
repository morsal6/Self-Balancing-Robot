/**
 * @file CommVerliesControl.hpp
 * @brief Wat er gebeurt als er te lang geen bericht komt (UC-CYD-03).
 */
#pragma once

#include "Display.hpp"
#include "RobotData.hpp"

/**
 * @class CommVerliesControl
 * @brief Toont dat de verbinding weg is.
 */
class CommVerliesControl {
private:
  RobotData& robotData;
  Display& display;

public:
  /// @brief Maakt de control met de objecten die hij gebruikt.
  CommVerliesControl(RobotData& robotData, Display& display);

  /// @brief Markeert de robotdata als verouderd en tekent het gezicht VERBINDING_VERLOREN.
  void voerUit();
};
