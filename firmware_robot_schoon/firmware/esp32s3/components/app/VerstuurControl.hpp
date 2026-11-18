/**
 * @file VerstuurControl.hpp
 * @brief Control-object en taak van UC-S3-04 Versturen (STD VerstuurControl).
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

/**
 * @class VerstuurControl
 * @brief Verstuurt iedere VERZENDPERIODE, en direct na verstuur(), een
 *        bericht naar de CYD.
 */
class VerstuurControl {
  /// Toestanden uit het STD van VerstuurControl.
  enum class State { Wachten };

  // Wachten wacht op twee dingen tegelijk. De after(VERZENDPERIODE) wordt
  // daarom een eigen software timer met een eventbit.
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

  // STD-interface. Wordt uitgevoerd in de taak van de aanroeper.

  /**
   * @brief Laat direct een bericht versturen (nieuwe fase of valteller).
   */
  void verstuur();

private:
  static void taskWrapper(void* pParam);
  static void verzendTimerCallback(TimerHandle_t xTimer);
  void main();
};
