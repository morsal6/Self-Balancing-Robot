/**
 * @file ScoreControl.cpp
 * @brief Implementatie van ScoreControl.
 */
#include "ScoreControl.hpp"

#include "Ontwerpparameters.hpp"
#include "TryReadQueue.hpp"
#include "ValtellerInfo.hpp"

#include <algorithm>

ScoreControl::ScoreControl(const char* taskName, UBaseType_t taskPriority,
                           uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                           Display& display, gpio_num_t touchIrqPin)
    : touchscreen("Touchscreen", TOUCHSCREEN_PRIORITEIT, TOUCHSCREEN_STACK,
                  CORE_COMMUNICATIE, touchIrqPin),
      display(display) {
  eventGroup = xEventGroupCreate();
  configASSERT(eventGroup != NULL);
  valtellerQueue = xQueueCreate(1, sizeof(ValtellerInfo));
  configASSERT(valtellerQueue != NULL);

  touchscreen.addListener(this);

  const BaseType_t gestart = xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes,
                                                     this, taskPriority, &taskHandle,
                                                     taskCoreNumber);
  configASSERT(gestart == pdPASS);
}

void ScoreControl::nieuweValteller(uint32_t sessieId, uint32_t valteller) {
  const ValtellerInfo info = {sessieId, valteller};
  xQueueOverwrite(valtellerQueue, &info);
  xEventGroupSetBits(eventGroup, eventBitValteller);
}

void ScoreControl::aangeraakt() {
  xEventGroupSetBits(eventGroup, eventBitAangeraakt);
}

void ScoreControl::taskWrapper(void* pParam) {
  static_cast<ScoreControl*>(pParam)->main();
}

void ScoreControl::main() {
  ValtellerInfo info;
  EventBits_t uxBits;

  score = 0;
  heeftReferentie = false;
  display.tekenScore(score);

  while (true) {
    switch (state) {
      case State::Wachten:
        // Zijn beide bits gezet, dan eerst de aanraking. Het andere bit blijft staan.
        uxBits = xEventGroupWaitBits(eventGroup, eventBitAangeraakt | eventBitValteller,
                                     pdFALSE, pdFALSE, portMAX_DELAY);
        if ((uxBits & eventBitAangeraakt) != 0) {
          xEventGroupClearBits(eventGroup, eventBitAangeraakt);
          score = 0;
          display.tekenScore(score);
        } else if ((uxBits & eventBitValteller) != 0) {
          if (tryReadQueue(valtellerQueue, info, eventGroup, eventBitValteller)) {
            if (!heeftReferentie) {  // d1: eerste bericht, nog niet tellen
              refSessieId = info.sessieId;
              refValteller = info.valteller;
              heeftReferentie = true;
            } else if (info.sessieId != refSessieId) {  // d2: S3 is herstart
              score = std::min(score + info.valteller, MAX_SCORE);
              refSessieId = info.sessieId;
              refValteller = info.valteller;
              display.tekenScore(score);
            } else if (info.valteller == refValteller) {  // d3
              // niet gevallen: niet opnieuw tekenen
            } else {
              score = std::min(score + (info.valteller - refValteller), MAX_SCORE);
              refValteller = info.valteller;
              display.tekenScore(score);
            }
          }
        }
        state = State::Wachten;
        break;
    }
  }
}
