/**
 * @file main.cpp
 * @brief Opstartcode van de ESP32-S3 Super Mini: maakt alle objecten aan.
 *
 * Volgorde: eerst de objecten zonder taak, dan de taken die wachten
 * (Mpu6050 in Uit, VerstuurControl in Wachten) en als laatste RobotControl,
 * omdat die direct begint en alle andere objecten dan al moeten bestaan.
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

// Pinnen, gemeten met de testcode.
// Motortest: BI hoog = vooruit (richting scherm), FI hoog = achteruit.
// Daarom is BI de A-ingang (vooruit) van MotorDriver.
constexpr gpio_num_t PIN_SDA = GPIO_NUM_8;       ///< MPU6050 SDA
constexpr gpio_num_t PIN_SCL = GPIO_NUM_9;       ///< MPU6050 SCL
constexpr gpio_num_t PIN_LINKS_A = GPIO_NUM_5;   ///< Linkermotor BI (vooruit)
constexpr gpio_num_t PIN_LINKS_B = GPIO_NUM_4;   ///< Linkermotor FI (achteruit)
constexpr gpio_num_t PIN_RECHTS_A = GPIO_NUM_7;  ///< Rechtermotor BI (vooruit)
constexpr gpio_num_t PIN_RECHTS_B = GPIO_NUM_6;  ///< Rechtermotor FI (achteruit)

extern "C" void app_main() {
  // Objecten zonder eigen taak. static: ze moeten blijven bestaan.
  static MotorDriver motorDriver(PIN_LINKS_A, PIN_LINKS_B, PIN_RECHTS_A, PIN_RECHTS_B);
  static EspNowZender espNowZender;
  static RobotFase robotFase;
  static Sessie sessie;
  static BalansData balansData;
  static BalansFilter balansFilter;
  static PidRegelaar pidRegelaar(PID_KP, PID_KI, PID_KD);

  // Taken die eerst wachten
  static Mpu6050 mpu6050("Mpu6050", MPU6050_PRIORITEIT, MPU6050_STACK, CORE_REGELING,
                         PIN_SDA, PIN_SCL);
  static VerstuurControl verstuurControl("VerstuurControl", VERSTUURCONTROL_PRIORITEIT,
                                         VERSTUURCONTROL_STACK, CORE_COMMUNICATIE,
                                         balansData, robotFase, sessie, espNowZender);

  // Passieve controls, uitgevoerd door RobotControl
  static InitControl initControl(sessie, motorDriver, mpu6050, espNowZender,
                                 balansFilter, pidRegelaar);
  static BalansControl balansControl(balansFilter, pidRegelaar, motorDriver, balansData);
  static BalansverliesControl balansverliesControl(motorDriver, pidRegelaar, sessie,
                                                   verstuurControl, balansFilter);
  static FoutControl foutControl(motorDriver, balansData);

  // Als laatste: RobotControl start direct met UC-S3-00.
  static RobotControl robotControl("RobotControl", ROBOTCONTROL_PRIORITEIT,
                                   ROBOTCONTROL_STACK, CORE_REGELING, robotFase,
                                   verstuurControl, initControl, balansControl,
                                   balansverliesControl, foutControl, mpu6050);

  // app_main doet verder niets. De objecten zijn static, dus app_main mag
  // eindigen; de main-taak wordt dan door ESP-IDF opgeruimd.
}
