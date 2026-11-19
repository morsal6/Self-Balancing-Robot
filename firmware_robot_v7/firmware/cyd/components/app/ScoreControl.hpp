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

/**
 * @class ScoreControl
 * @brief Telt de valteller van de S3 op en zet de score op 0 bij een aanraking.
 */
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
  /**
   * @brief Maakt de taak en het Touchscreen aan.
   * @param taskName naam van de taak
   * @param taskPriority prioriteit van de taak
   * @param taskStackSizeBytes stackgrootte in bytes
   * @param taskCoreNumber core waarop de taak draait
   * @param display het Display voor de score
   * @param touchIrqPin T_IRQ-pin van de XPT2046
   */
  ScoreControl(const char* taskName, UBaseType_t taskPriority,
               uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
               Display& display, gpio_num_t touchIrqPin);

  /// @brief Geeft de valteller uit een bericht door.
  void nieuweValteller(uint32_t sessieId, uint32_t valteller);

  /// @brief Scherm aangeraakt: score op 0.
  void aangeraakt() override;

private:
  static void taskWrapper(void* pParam);
  void main();
};
