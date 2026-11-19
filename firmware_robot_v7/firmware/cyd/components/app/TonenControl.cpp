/**
 * @file TonenControl.cpp
 * @brief Implementatie van TonenControl.
 */
#include "TonenControl.hpp"

#include "Ontwerpparameters.hpp"
#include "TryReadQueue.hpp"

#include "esp_log.h"

#include <algorithm>
#include <cmath>

namespace {
const char* TAG = "TonenControl";
}

TonenControl::TonenControl(const char* taskName, UBaseType_t taskPriority,
                           uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                           RobotData& robotData, Display& display,
                           ScoreControl& scoreControl, CommVerliesControl& commVerliesControl)
    : robotData(robotData), display(display), scoreControl(scoreControl),
      commVerliesControl(commVerliesControl) {
  eventGroup = xEventGroupCreate();
  configASSERT(eventGroup != NULL);
  berichtQueue = xQueueCreate(1, sizeof(Bericht));
  configASSERT(berichtQueue != NULL);

  commTimer = xTimerCreate("CommTimer", pdMS_TO_TICKS(COMM_TIMEOUT), pdFALSE,
                           this, commTimerCallback);
  configASSERT(commTimer != NULL);

  espNowOntvanger.addListener(this);

  const BaseType_t gestart = xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes,
                                                     this, taskPriority, &taskHandle,
                                                     taskCoreNumber);
  configASSERT(gestart == pdPASS);
}

void TonenControl::berichtOntvangen(Bericht bericht) {
  xQueueOverwrite(berichtQueue, &bericht);
  xEventGroupSetBits(eventGroup, eventBitBericht);
}

bool TonenControl::isGeldig(Bericht bericht) {
  const bool faseBestaat = static_cast<uint8_t>(bericht.fase) <= static_cast<uint8_t>(Fase::FOUT);
  const bool waardeBestaat = std::isfinite(bericht.balanswaarde);
  const bool isNieuw = (bericht.sessieId != laatsteSessieId) ||
                       (bericht.volgnummer > laatsteVolgnummer);
  return faseBestaat && waardeBestaat && (bericht.sessieId != 0) && isNieuw;
}

Expressie TonenControl::kiesExpressie(Bericht bericht) {
  Expressie expressie = {ExpressieSoort::BALANCEREN, 0};
  switch (bericht.fase) {
    case Fase::KALIBREREN:
      expressie.soort = ExpressieSoort::OPSTARTEN;
      break;
    case Fase::BALANCEREN: {
      expressie.soort = (std::fabs(bericht.balanswaarde) > SCHRIKGRENS)
                            ? ExpressieSoort::SCHRIKKEN
                            : ExpressieSoort::BALANCEREN;

      // Pupillen kijken mee met de kanteling en de rijrichting.
      const float oogX = bericht.balanswaarde * OOG_SCHAAL + bericht.richting * RICHTING_OOGX;
      expressie.oogX = static_cast<int8_t>(std::clamp(oogX, static_cast<float>(-MAX_OOGX),
                                                      static_cast<float>(MAX_OOGX)));
      break;
    }
    case Fase::BALANSVERLIES:
      expressie.soort = ExpressieSoort::GEVALLEN;
      break;
    case Fase::FOUT:
      expressie.soort = ExpressieSoort::FOUT;
      break;
  }
  return expressie;
}

void TonenControl::taskWrapper(void* pParam) {
  static_cast<TonenControl*>(pParam)->main();
}

void TonenControl::commTimerCallback(TimerHandle_t xTimer) {
  TonenControl* instance = static_cast<TonenControl*>(pvTimerGetTimerID(xTimer));
  xEventGroupSetBits(instance->eventGroup, instance->eventBitCommTimer);
}

void TonenControl::main() {
  Bericht bericht;
  EventBits_t uxBits;

  display.init();
  espNowOntvanger.init();
  xTimerStart(commTimer, portMAX_DELAY);

  while (true) {
    switch (state) {
      case State::Wachten:
        uxBits = xEventGroupWaitBits(eventGroup, eventBitBericht | eventBitCommTimer,
                                     pdFALSE, pdFALSE, portMAX_DELAY);

        if ((uxBits & eventBitBericht) != 0) {
          if (tryReadQueue(berichtQueue, bericht, eventGroup, eventBitBericht)) {
            if (!isGeldig(bericht)) {
              // ongeldig of oud bericht: negeren
            } else {
              if (laatsteSessieId != bericht.sessieId) {
                ESP_LOGI(TAG, "Verbinding met robot: sessie %08X", (unsigned)bericht.sessieId);
              }
              xEventGroupClearBits(eventGroup, eventBitCommTimer);  // kan al gezet zijn door de timer
              xTimerReset(commTimer, portMAX_DELAY);
              laatsteSessieId = bericht.sessieId;
              laatsteVolgnummer = bericht.volgnummer;
              robotData.schrijf(bericht);
              display.tekenGezicht(kiesExpressie(bericht));
              scoreControl.nieuweValteller(bericht.sessieId, bericht.valteller);
            }
          }
        } else if ((uxBits & eventBitCommTimer) != 0) {
          xEventGroupClearBits(eventGroup, eventBitCommTimer);
          ESP_LOGW(TAG, "Geen geldig bericht binnen %u ms: communicatieverlies",
                   (unsigned)COMM_TIMEOUT);
          commVerliesControl.voerUit();
        }
        state = State::Wachten;
        break;
    }
  }
}
