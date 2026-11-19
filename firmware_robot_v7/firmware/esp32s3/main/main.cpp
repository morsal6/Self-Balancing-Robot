/**
 * @file main.cpp
 * @brief Maakt alle objecten van de robot aan.
 */
#include "BalansControl.hpp"
#include "BalansData.hpp"
#include "BalansFilter.hpp"
#include "BalansverliesControl.hpp"
#include "EspNowZender.hpp"
#include "FoutControl.hpp"
#include "InitControl.hpp"
#include "MotorDriver.hpp"
#include "Mpu6050.hpp"
#include "Ontwerpparameters.hpp"
#include "PidRegelaar.hpp"
#include "RobotControl.hpp"
#include "RobotFase.hpp"
#include "Sessie.hpp"
#include "VerstuurControl.hpp"

#include "driver/gpio.h"

constexpr gpio_num_t PIN_SDA = GPIO_NUM_8;
constexpr gpio_num_t PIN_SCL = GPIO_NUM_9;
constexpr gpio_num_t PIN_LINKS_A = GPIO_NUM_5;
constexpr gpio_num_t PIN_LINKS_B = GPIO_NUM_4;
constexpr gpio_num_t PIN_RECHTS_A = GPIO_NUM_7;
constexpr gpio_num_t PIN_RECHTS_B = GPIO_NUM_6;

extern "C" void app_main() {
  static MotorDriver motorDriver(PIN_LINKS_A, PIN_LINKS_B, PIN_RECHTS_A, PIN_RECHTS_B);
  static EspNowZender espNowZender;
  static RobotFase robotFase;
  static Sessie sessie;
  static BalansData balansData;
  static BalansFilter balansFilter;
  static PidRegelaar pidRegelaar(PID_KP, PID_KI, PID_KD);

  // Taken
  static Mpu6050 mpu6050("Mpu6050", MPU6050_PRIORITEIT, MPU6050_STACK, CORE_REGELING,
                         PIN_SDA, PIN_SCL);
  static VerstuurControl verstuurControl("VerstuurControl", VERSTUURCONTROL_PRIORITEIT,
                                         VERSTUURCONTROL_STACK, CORE_COMMUNICATIE,
                                         balansData, robotFase, sessie, espNowZender);

  // Controls zonder eigen taak, RobotControl roept ze aan
  static InitControl initControl(sessie, motorDriver, mpu6050, espNowZender,
                                 balansFilter, pidRegelaar);
  static BalansControl balansControl(balansFilter, pidRegelaar, motorDriver, balansData);
  static BalansverliesControl balansverliesControl(motorDriver, pidRegelaar, sessie,
                                                   verstuurControl, balansFilter);
  static FoutControl foutControl(motorDriver, balansData);

  // RobotControl als laatste, die begint meteen
  static RobotControl robotControl("RobotControl", ROBOTCONTROL_PRIORITEIT,
                                   ROBOTCONTROL_STACK, CORE_REGELING, robotFase,
                                   verstuurControl, initControl, balansControl,
                                   balansverliesControl, foutControl, mpu6050);
}
