/**
 * @file TonenControl.hpp
 * @brief Control-object en taak van UC-CYD-01 Tonen (STD TonenControl).
 */
#pragma once

#include "Bericht.hpp"
#include "BerichtListener.hpp"
#include "CommVerliesControl.hpp"
#include "Display.hpp"
#include "EspNowOntvanger.hpp"
#include "Expressie.hpp"
#include "RobotData.hpp"
#include "ScoreControl.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include <cstdint>

/**
 * @class TonenControl
 * @brief Ontvangt de berichten van de ESP32-S3, kiest het gezicht en geeft
 *        de valteller door aan ScoreControl. Voert UC-CYD-03 uit als er
 *        binnen COMM_TIMEOUT geen geldig bericht komt.
 */
class TonenControl : public BerichtListener {
  /// Toestanden uit het STD van TonenControl.
  enum class State { Wachten };

  static constexpr EventBits_t eventBitBericht = (1 << 0);
  static constexpr EventBits_t eventBitCommTimer = (1 << 1);

private:
  State state = State::Wachten;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;
  QueueHandle_t berichtQueue = nullptr;  ///< Lengte 1: alleen het laatste bericht telt
  TimerHandle_t commTimer = nullptr;

  uint32_t laatsteSessieId = 0;
  uint32_t laatsteVolgnummer = 0;
  EspNowOntvanger espNowOntvanger;
  RobotData& robotData;
  Display& display;
  ScoreControl& scoreControl;
  CommVerliesControl& commVerliesControl;

public:
  TonenControl(const char* taskName, UBaseType_t taskPriority,
               uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
               RobotData& robotData, Display& display, ScoreControl& scoreControl,
               CommVerliesControl& commVerliesControl);

  // STD-interface. Wordt uitgevoerd in de taak van de aanroeper (WiFi-taak).

  /**
   * @brief Ontvangt een bericht van de EspNowOntvanger.
   * @param bericht het ontvangen bericht
   */
  void berichtOntvangen(Bericht bericht) override;

private:
  /**
   * @brief Controleert of een bericht bruikbaar en nieuw is.
   * @param bericht het ontvangen bericht
   * @return false als het bericht ongeldig, dubbel of verouderd is
   */
  bool isGeldig(Bericht bericht);

  /**
   * @brief Kiest het gezicht bij een bericht (UC-CYD-01 stap 5 en 6).
   * @param bericht een geldig bericht
   * @return de te tekenen expressie
   */
  Expressie kiesExpressie(Bericht bericht);

  static void taskWrapper(void* pParam);
  static void commTimerCallback(TimerHandle_t xTimer);
  void main();
};
