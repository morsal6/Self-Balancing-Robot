/**
 * @file CommVerliesControl.hpp
 * @brief Control-object van UC-CYD-03 Communicatieverlies tonen.
 */
#pragma once

#include "Display.hpp"
#include "RobotData.hpp"

/**
 * @class CommVerliesControl
 * @brief "Dirigent" van UC-CYD-03. Passief: wordt uitgevoerd door
 *        TonenControl, in de taak van TonenControl.
 */
class CommVerliesControl {
private:
  RobotData& robotData;
  Display& display;

public:
  CommVerliesControl(RobotData& robotData, Display& display);

  /**
   * @brief Markeert de gegevens als verouderd en toont de
   *        communicatieverlies-expressie. De score blijft staan.
   */
  void voerUit();
};
