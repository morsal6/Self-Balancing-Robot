/**
 * @file ScoreControl.hpp
 * @brief Control-object en taak van UC-CYD-02 Score bijhouden (STD ScoreControl).
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
 * @brief Houdt de score bij op basis van de valteller van de ESP32-S3.
 *        Een tik op het touchscreen start een nieuw spel.
 */
class ScoreControl : public TouchListener {
  /// Toestanden uit het STD van ScoreControl.
  enum class State { Wachten };

  static constexpr EventBits_t eventBitValteller = (1 << 0);
  static constexpr EventBits_t eventBitAangeraakt = (1 << 1);

private:
  State state = State::Wachten;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;
  QueueHandle_t valtellerQueue = nullptr;  ///< Lengte 1: alleen de laatste valteller telt

  uint32_t score = 0;
  uint32_t refSessieId = 0;
  uint32_t refValteller = 0;
  bool heeftReferentie = false;
  Touchscreen touchscreen;
  Display& display;

public:
  /**
   * @brief Maakt het touchscreen aan, meldt zich daar als listener aan en
   *        start de taak.
   * @param taskName naam van de taak
   * @param taskPriority prioriteit van de taak
   * @param taskStackSizeBytes stackgrootte in bytes
   * @param taskCoreNumber core waarop de taak draait
   * @param display het scherm voor de score
   * @param touchIrqPin GPIO van T_IRQ van het touchscreen
   */
  ScoreControl(const char* taskName, UBaseType_t taskPriority,
               uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
               Display& display, gpio_num_t touchIrqPin);

  // STD-interface. Wordt uitgevoerd in de taak van de aanroeper.

  /**
   * @brief Geeft een nieuwe valteller door (vanuit TonenControl).
   * @param sessieId sessie-ID van de ESP32-S3
   * @param valteller valteller binnen die sessie
   */
  void nieuweValteller(uint32_t sessieId, uint32_t valteller);

  /**
   * @brief Het touchscreen is aangeraakt: nieuw spel.
   */
  void aangeraakt() override;

private:
  static void taskWrapper(void* pParam);
  void main();
};
