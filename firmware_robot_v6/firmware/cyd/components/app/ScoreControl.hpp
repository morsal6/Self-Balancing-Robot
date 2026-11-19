/**
 * @file ScoreControl.hpp
 * @brief STD ScoreControl: houdt de score bij (UC-CYD-02).
 */
#pragma once

#include "Display.hpp"
#include "TouchListener.hpp"
#include "Touchscreen.hpp"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <cstdint>

/// @brief Telt de valteller van de S3 op en zet de score op 0 bij een aanraking.
class ScoreControl : public TouchListener {
  enum class State { Wachten };

  static constexpr EventBits_t eventBitValteller = (1 << 0);
  static constexpr EventBits_t eventBitAangeraakt = (1 << 1);

private:
  State state = State::Wachten;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;
  QueueHandle_t valtellerQueue = nullptr;

  uint32_t score = 0;
  uint32_t refSessieId = 0;
  uint32_t refValteller = 0;
  bool heeftReferentie = false;
  Touchscreen touchscreen;
  Display& display;

public:
  ScoreControl(const char* taskName, UBaseType_t taskPriority,
               uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
               Display& display, gpio_num_t touchIrqPin);

  void nieuweValteller(uint32_t sessieId, uint32_t valteller);

  void aangeraakt() override;

private:
  static void taskWrapper(void* pParam);
  void main();
};
