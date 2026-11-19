/**
 * @file main.cpp
 * @brief Maakt alle objecten van de CYD aan.
 */
#include "CommVerliesControl.hpp"
#include "Display.hpp"
#include "Ontwerpparameters.hpp"
#include "RobotData.hpp"
#include "ScoreControl.hpp"
#include "TonenControl.hpp"

#include "driver/gpio.h"

constexpr int LCD_MOSI = 13;
constexpr int LCD_SCLK = 14;
constexpr int LCD_CS = 15;
constexpr int LCD_DC = 2;
constexpr int LCD_RST = -1;
#ifdef DISPLAY28
constexpr int LCD_BL = 21;
#else
constexpr int LCD_BL = 27;
#endif
constexpr gpio_num_t TOUCH_IRQ = GPIO_NUM_36;  // T_IRQ van de XPT2046

extern "C" void app_main() {
  // Display eerst: die heeft de grootste blokken DMA-geheugen nodig.
  static Display display("Display", DISPLAY_PRIORITEIT, DISPLAY_STACK, CORE_TEKENEN,
                         LCD_MOSI, LCD_SCLK, LCD_CS, LCD_DC, LCD_RST, LCD_BL);
  static RobotData robotData;
  static CommVerliesControl commVerliesControl(robotData, display);
  static ScoreControl scoreControl("ScoreControl", SCORECONTROL_PRIORITEIT,
                                   SCORECONTROL_STACK, CORE_COMMUNICATIE, display, TOUCH_IRQ);

  static TonenControl tonenControl("TonenControl", TONENCONTROL_PRIORITEIT,
                                   TONENCONTROL_STACK, CORE_COMMUNICATIE, robotData,
                                   display, scoreControl, commVerliesControl);
}
