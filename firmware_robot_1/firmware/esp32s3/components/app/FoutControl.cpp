/**
 * @file FoutControl.cpp
 * @brief Implementatie van FoutControl.
 */
#include "FoutControl.hpp"

FoutControl::FoutControl(MotorDriver& motorDriver, BalansData& balansData)
    : motorDriver(motorDriver), balansData(balansData) {}

void FoutControl::handelAf() {
  motorDriver.stuur(0, 0);
  balansData.markeerOngeldig();
}
