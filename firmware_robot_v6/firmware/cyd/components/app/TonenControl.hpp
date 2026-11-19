/**
 * @file TonenControl.hpp
 * @brief STD TonenControl: verwerkt berichten van de robot (UC-CYD-01).
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

/// @brief Kiest het gezicht bij elk geldig bericht en merkt communicatieverlies op.
class TonenControl : public BerichtListener {
  enum class State { Wachten };

  static constexpr EventBits_t eventBitBericht = (1 << 0);
  static constexpr EventBits_t eventBitCommTimer = (1 << 1);

private:
  State state = State::Wachten;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;
  QueueHandle_t berichtQueue = nullptr;
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

  void berichtOntvangen(Bericht bericht) override;

private:
  bool isGeldig(Bericht bericht);

  Expressie kiesExpressie(Bericht bericht);

  static void taskWrapper(void* pParam);
  static void commTimerCallback(TimerHandle_t xTimer);
  void main();
};
