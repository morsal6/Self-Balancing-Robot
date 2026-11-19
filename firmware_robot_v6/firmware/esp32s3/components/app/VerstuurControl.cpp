/**
 * @file VerstuurControl.cpp
 * @brief Implementatie van VerstuurControl.
 */
#include "VerstuurControl.hpp"

#include "Bericht.hpp"
#include "Ontwerpparameters.hpp"

VerstuurControl::VerstuurControl(const char* taskName, UBaseType_t taskPriority,
                                 uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                                 BalansData& balansData, RobotFase& robotFase,
                                 Sessie& sessie, EspNowZender& espNowZender)
    : balansData(balansData), robotFase(robotFase), sessie(sessie),
      espNowZender(espNowZender) {
  eventGroup = xEventGroupCreate();
  configASSERT(eventGroup != NULL);

  verzendTimer = xTimerCreate("VerzendTimer", pdMS_TO_TICKS(VERZENDPERIODE), pdFALSE,
                              this, verzendTimerCallback);
  configASSERT(verzendTimer != NULL);

  const BaseType_t gestart = xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes,
                                                     this, taskPriority, &taskHandle,
                                                     taskCoreNumber);
  configASSERT(gestart == pdPASS);
}

void VerstuurControl::verstuur() {
  xEventGroupSetBits(eventGroup, eventBitVerstuur);
}

void VerstuurControl::taskWrapper(void* pParam) {
  static_cast<VerstuurControl*>(pParam)->main();
}

void VerstuurControl::verzendTimerCallback(TimerHandle_t xTimer) {
  VerstuurControl* instance = static_cast<VerstuurControl*>(pvTimerGetTimerID(xTimer));
  xEventGroupSetBits(instance->eventGroup, instance->eventBitVerzendTimer);
}

void VerstuurControl::main() {
  Bericht bericht;
  float balanswaarde = 0.0f;
  int8_t richting = 0;

  volgnummer = 0;

  while (true) {
    switch (state) {
      case State::Wachten:
        // entry: timer van after(VERZENDPERIODE) starten
        xEventGroupClearBits(eventGroup, eventBitVerzendTimer);
        xTimerStart(verzendTimer, portMAX_DELAY);

        xEventGroupWaitBits(eventGroup, eventBitVerzendTimer | eventBitVerstuur,
                            pdTRUE, pdFALSE, portMAX_DELAY);

        // m1: de timer en verstuur() komen hier samen
        balansData.lees(balanswaarde, richting);
        bericht.sessieId = sessie.getSessieId();
        bericht.volgnummer = ++volgnummer;
        bericht.fase = robotFase.getFase();
        bericht.balanswaarde = balanswaarde;
        bericht.richting = richting;
        bericht.valteller = sessie.getValteller();
        espNowZender.verstuur(bericht);
        state = State::Wachten;
        break;
    }
  }
}
