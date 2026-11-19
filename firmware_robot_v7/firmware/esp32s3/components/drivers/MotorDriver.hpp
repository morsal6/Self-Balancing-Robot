/**
 * @file MotorDriver.hpp
 * @brief Stuurt de twee motoren via de H-brug met PWM.
 */
#pragma once

#include "driver/gpio.h"

#include <cstdint>

/**
 * @class MotorDriver
 * @brief Stuurt twee motoren via de H-brug met PWM (LEDC).
 */
class MotorDriver {
private:
  int32_t vorigeLinks = 0;
  int32_t vorigeRechts = 0;

public:
  /**
   * @brief Stelt de PWM-timer en de vier kanalen in.
   * @param linksA pin vooruit links
   * @param linksB pin achteruit links
   * @param rechtsA pin vooruit rechts
   * @param rechtsB pin achteruit rechts
   */
  MotorDriver(gpio_num_t linksA, gpio_num_t linksB,
              gpio_num_t rechtsA, gpio_num_t rechtsB);

  /**
   * @brief Positief is vooruit, 0 is stil (direct).
   * De uitgang verandert per aanroep hoogstens MOTOR_MAX_STAP.
   * @param links uitgang links, -MAX_MOTORUITGANG..MAX_MOTORUITGANG
   * @param rechts uitgang rechts
   * @return false als het schrijven naar de PWM mislukt
   */
  bool stuur(int16_t links, int16_t rechts);
};
