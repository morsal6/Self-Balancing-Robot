/**
 * @file CommVerliesControl.cpp
 * @brief Implementatie van CommVerliesControl.
 */
#include "CommVerliesControl.hpp"

CommVerliesControl::CommVerliesControl(RobotData& robotData, Display& display)
    : robotData(robotData), display(display) {}

void CommVerliesControl::voerUit() {
  robotData.markeerVerouderd();
  display.tekenGezicht(Expressie{ExpressieSoort::VERBINDING_VERLOREN, 0});
}
