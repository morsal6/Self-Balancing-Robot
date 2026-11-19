/**
 * @file MotorDriver.hpp
 * @brief Boundary-object voor de ZK5AD-h-brug met de twee TT130-motoren.
 */
#pragma once

#include "driver/gpio.h"

#include <cstdint>

/**
 * @class MotorDriver
 * @brief Stuurt de linker- en rechtermotor aan via de ZK5AD.
 *
 * De ZK5AD heeft per motor twee ingangen. Ingang A is de ingang die de robot
 * vooruit laat rijden. Vooruit: PWM op A, B laag. Achteruit: andersom.
 * Beide laag: de motor loopt vrij uit.
 *
 * Voorbeeld:
 * @code
 * MotorDriver motorDriver(GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_7, GPIO_NUM_6);
 * motorDriver.stuur(800, 800);  // beide motoren vooruit
 * motorDriver.stuur(0, 0);      // beide motoren direct stil
 * @endcode
 */
class MotorDriver {
private:
  int32_t vorigeLinks = 0;   ///< Laatste uitgang links, voor MOTOR_MAX_STAP
  int32_t vorigeRechts = 0;  ///< Laatste uitgang rechts, voor MOTOR_MAX_STAP

public:
  /**
   * @brief Configureert de PWM-uitgangen. Beide motoren staan stil.
   * @param linksA ingang van de linkermotor die vooruit geeft
   * @param linksB ingang van de linkermotor die achteruit geeft
   * @param rechtsA ingang van de rechtermotor die vooruit geeft
   * @param rechtsB ingang van de rechtermotor die achteruit geeft
   */
  MotorDriver(gpio_num_t linksA, gpio_num_t linksB,
              gpio_num_t rechtsA, gpio_num_t rechtsB);

  /**
   * @brief Stuurt beide motoren aan.
   *
   * Waarden worden begrensd op [-MAX_MOTORUITGANG, MAX_MOTORUITGANG].
   * Positief is vooruit, negatief achteruit, 0 is stil. Onder
   * MOTOR_STARTWAARDE draaien de motoren niet; PidRegelaar houdt daar
   * rekening mee. De uitgang verandert per aanroep hoogstens MOTOR_MAX_STAP;
   * 0 gaat direct.
   *
   * @param links commando voor de linkermotor
   * @param rechts commando voor de rechtermotor
   * @return true als het commando naar de hardware is geschreven
   */
  bool stuur(int16_t links, int16_t rechts);
};
