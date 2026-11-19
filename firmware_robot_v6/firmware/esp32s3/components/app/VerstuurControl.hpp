/**
 * @file VerstuurControl.hpp
 * @brief STD VerstuurControl: stuurt de status naar de CYD.
 */
#pragma once

#include "BalansData.hpp"
#include "EspNowZender.hpp"
#include "RobotFase.hpp"
#include "Sessie.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include <cstdint>

/// @brief Stuurt elke VERZENDPERIODE een bericht, of direct na verstuur().
class VerstuurControl {
  enum class State { Wachten };

  static constexpr EventBits_t eventBitVerstuur = (1 << 0);
  static constexpr EventBits_t eventBitVerzendTimer = (1 << 1);

private:
  State state = State::Wachten;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;
  TimerHandle_t verzendTimer = nullptr;

  uint32_t volgnummer = 0;
  BalansData& balansData;
  RobotFase& robotFase;
  Sessie& sessie;
  EspNowZender& espNowZender;

public:
  VerstuurControl(const char* taskName, UBaseType_t taskPriority,
                  uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                  BalansData& balansData, RobotFase& robotFase, Sessie& sessie,
                  EspNowZender& espNowZender);

  void verstuur();

private:
  static void taskWrapper(void* pParam);
  static void verzendTimerCallback(TimerHandle_t xTimer);
  void main();
};
