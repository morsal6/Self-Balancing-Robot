/**
 * @file ScoreControl.cpp
 * @brief Implementatie van het STD van ScoreControl.
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

  // Meld mezelf als TouchListener aan bij het touchscreen.
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

  // Pijl uit de initial node
  score = 0;
  heeftReferentie = false;
  display.tekenScore(score);

  while (true) {
    switch (state) {
      case State::Wachten:
        uxBits = xEventGroupWaitBits(eventGroup, eventBitAangeraakt | eventBitValteller,
                                     pdFALSE, pdFALSE, portMAX_DELAY);
        if ((uxBits & eventBitAangeraakt) != 0) {
          // Nieuw spel. refValteller is al de laatst ontvangen valteller.
          xEventGroupClearBits(eventGroup, eventBitAangeraakt);
          score = 0;
          display.tekenScore(score);
        } else if ((uxBits & eventBitValteller) != 0) {
          if (tryReadQueue(valtellerQueue, info, eventGroup, eventBitValteller)) {
            // d1
            if (!heeftReferentie) {
              refSessieId = info.sessieId;
              refValteller = info.valteller;
              heeftReferentie = true;
            } else if (info.sessieId != refSessieId) {  // d2
              // De ESP32-S3 is opnieuw opgestart: tel de nieuwe valteller helemaal mee.
              score = std::min(score + info.valteller, MAX_SCORE);
              refSessieId = info.sessieId;
              refValteller = info.valteller;
              display.tekenScore(score);
            } else if (info.valteller == refValteller) {  // d3
              // Geen nieuw balansverlies. Niet opnieuw tekenen: dit gebeurt bij
              // bijna elk bericht (20 keer per seconde) en zou het Display
              // onnodig bezig houden.
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
