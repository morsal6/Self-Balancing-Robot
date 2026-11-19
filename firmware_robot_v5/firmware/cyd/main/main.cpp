/**
 * @file main.cpp
 * @brief Opstartcode van de CYD: maakt alle objecten aan.
 *
 * Volgorde: eerst Display (wacht in Uit), dan de passieve objecten, dan
 * ScoreControl (met zijn Touchscreen) en als laatste TonenControl, omdat die
 * direct begint en alle andere objecten dan al moeten bestaan.
 */
#include "CommVerliesControl.hpp"
#include "Display.hpp"
#include "Ontwerpparameters.hpp"
#include "RobotData.hpp"
#include "ScoreControl.hpp"
#include "TonenControl.hpp"

#include "driver/gpio.h"

// Pinnen van de CYD (ESP32-2432S028), zie TI-S3/software/CYD/README.md.
constexpr int LCD_MOSI = 13;  ///< Master Out Slave In
constexpr int LCD_SCLK = 14;  ///< Serial Clock
constexpr int LCD_CS = 15;    ///< Chip Select (actief laag)
constexpr int LCD_DC = 2;     ///< Data/-Command
constexpr int LCD_RST = -1;   ///< Reset (niet aangesloten)
#ifdef DISPLAY28
constexpr int LCD_BL = 21;    ///< Backlight van het 2,8 inch scherm
#else
constexpr int LCD_BL = 27;    ///< Backlight van het 2,4 inch scherm
#endif
constexpr gpio_num_t TOUCH_IRQ = GPIO_NUM_36;  ///< T_IRQ van de XPT2046

extern "C" void app_main() {
  static Display display("Display", DISPLAY_PRIORITEIT, DISPLAY_STACK, CORE_TEKENEN,
                         LCD_MOSI, LCD_SCLK, LCD_CS, LCD_DC, LCD_RST, LCD_BL);
  static RobotData robotData;
  static CommVerliesControl commVerliesControl(robotData, display);
  static ScoreControl scoreControl("ScoreControl", SCORECONTROL_PRIORITEIT,
                                   SCORECONTROL_STACK, CORE_COMMUNICATIE, display, TOUCH_IRQ);

  // Als laatste: TonenControl start direct met UC-CYD-01.
  static TonenControl tonenControl("TonenControl", TONENCONTROL_PRIORITEIT,
                                   TONENCONTROL_STACK, CORE_COMMUNICATIE, robotData,
                                   display, scoreControl, commVerliesControl);

  // app_main doet verder niets. De objecten zijn static, dus app_main mag
  // eindigen; de main-taak wordt dan door ESP-IDF opgeruimd.
}
