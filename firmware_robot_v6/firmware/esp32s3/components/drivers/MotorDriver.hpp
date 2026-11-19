/**
 * @file MotorDriver.hpp
 * @brief Stuurt de twee motoren via de H-brug met PWM.
 */
#pragma once

#include "driver/gpio.h"

#include <cstdint>

class MotorDriver {
private:
  int32_t vorigeLinks = 0;
  int32_t vorigeRechts = 0;

public:
  MotorDriver(gpio_num_t linksA, gpio_num_t linksB,
              gpio_num_t rechtsA, gpio_num_t rechtsB);

  /**
   * Positief is vooruit, 0 is stil (direct).
   * De uitgang verandert per aanroep hoogstens MOTOR_MAX_STAP.
   */
  bool stuur(int16_t links, int16_t rechts);
};
