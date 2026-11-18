/**
 * @file VerstuurControl.cpp
 * @brief Implementatie van het STD van VerstuurControl.
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

  // Timer van de after(VERZENDPERIODE). One shot; wordt bij elke entry van
  // Wachten opnieuw gestart.
  verzendTimer = xTimerCreate("VerzendTimer", pdMS_TO_TICKS(VERZENDPERIODE), pdFALSE,
                              this, verzendTimerCallback);
  configASSERT(verzendTimer != NULL);

  xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes, this,
                          taskPriority, &taskHandle, taskCoreNumber);
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

  // Pijl uit de initial node
  volgnummer = 0;

  while (true) {
    switch (state) {
      case State::Wachten:
        // entry: start de timer van after(VERZENDPERIODE)
        xEventGroupClearBits(eventGroup, eventBitVerzendTimer);
        xTimerStart(verzendTimer, portMAX_DELAY);

        // after(VERZENDPERIODE) en eventBitVerstuur gaan via de merge node m1
        // naar dezelfde actie, dus beide bits worden gewist (pdTRUE).
        xEventGroupWaitBits(eventGroup, eventBitVerzendTimer | eventBitVerstuur,
                            pdTRUE, pdFALSE, portMAX_DELAY);

        // m1
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
