/**
 * @file RobotControl.cpp
 * @brief Implementatie van het STD van RobotControl.
 */
#include "RobotControl.hpp"

#include "Resultaat.hpp"
#include "TryReadQueue.hpp"

#include "esp_log.h"

namespace {
const char* TAG = "RobotControl";
}

RobotControl::RobotControl(const char* taskName, UBaseType_t taskPriority,
                           uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                           RobotFase& robotFase, VerstuurControl& verstuurControl,
                           InitControl& initControl, BalansControl& balansControl,
                           BalansverliesControl& balansverliesControl,
                           FoutControl& foutControl, Mpu6050& mpu6050)
    : robotFase(robotFase), verstuurControl(verstuurControl), initControl(initControl),
      balansControl(balansControl), balansverliesControl(balansverliesControl),
      foutControl(foutControl) {
  eventGroup = xEventGroupCreate();
  configASSERT(eventGroup != NULL);
  metingQueue = xQueueCreate(1, sizeof(Meting));
  configASSERT(metingQueue != NULL);

  // Meld mezelf als MetingListener aan bij de Mpu6050.
  mpu6050.addListener(this);

  // Start de thread van deze taak (als laatste).
  xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes, this,
                          taskPriority, &taskHandle, taskCoreNumber);
}

void RobotControl::metingGedaan(Meting meting) {
  xQueueOverwrite(metingQueue, &meting);
  xEventGroupSetBits(eventGroup, eventBitMeting);
}

void RobotControl::taskWrapper(void* pParam) {
  static_cast<RobotControl*>(pParam)->main();
}

void RobotControl::main() {
  Meting meting;
  Resultaat resultaat;

  // Pijl uit de initial node
  robotFase.setFase(Fase::KALIBREREN);
  verstuurControl.verstuur();
  // d0
  if (initControl.start()) {
    ESP_LOGI(TAG, "Initialiseren: houd de robot 2 s stil rechtop (fase KALIBREREN)");
    state = State::Initialiseren;
  } else {
    state = State::Fout;  // via merge node mf
  }

  while (true) {
    switch (state) {
      case State::Initialiseren:
        do {
          xEventGroupWaitBits(eventGroup, eventBitMeting, pdFALSE, pdFALSE, portMAX_DELAY);
        } while (!tryReadQueue(metingQueue, meting, eventGroup, eventBitMeting));
        resultaat = initControl.verwerkMeting(meting);
        // d1
        if (resultaat == Resultaat::KLAAR) {
          ESP_LOGI(TAG, "Initialiseren -> Balanceren (nulpunt bepaald)");
          robotFase.setFase(Fase::BALANCEREN);
          verstuurControl.verstuur();
          balansControl.start();
          state = State::Balanceren;
        } else if (resultaat == Resultaat::FOUT) {
          state = State::Fout;  // via merge node mf
        } else {
          state = State::Initialiseren;
        }
        break;

      case State::Balanceren:
        do {
          xEventGroupWaitBits(eventGroup, eventBitMeting, pdFALSE, pdFALSE, portMAX_DELAY);
        } while (!tryReadQueue(metingQueue, meting, eventGroup, eventBitMeting));
        resultaat = balansControl.verwerkMeting(meting);
        // d2
        if (resultaat == Resultaat::BALANSVERLIES) {
          ESP_LOGI(TAG, "Balanceren -> Balansverlies (omgevallen)");
          robotFase.setFase(Fase::BALANSVERLIES);
          verstuurControl.verstuur();
          balansverliesControl.start();
          state = State::Balansverlies;
        } else if (resultaat == Resultaat::FOUT) {
          state = State::Fout;  // via merge node mf
        } else {
          state = State::Balanceren;
        }
        break;

      case State::Balansverlies:
        do {
          xEventGroupWaitBits(eventGroup, eventBitMeting, pdFALSE, pdFALSE, portMAX_DELAY);
        } while (!tryReadQueue(metingQueue, meting, eventGroup, eventBitMeting));
        resultaat = balansverliesControl.verwerkMeting(meting);
        // d3
        if (resultaat == Resultaat::KLAAR) {
          ESP_LOGI(TAG, "Balansverlies -> Balanceren (weer rechtop en stil)");
          robotFase.setFase(Fase::BALANCEREN);
          verstuurControl.verstuur();
          balansControl.start();
          state = State::Balanceren;
        } else if (resultaat == Resultaat::FOUT) {
          state = State::Fout;  // via merge node mf
        } else {
          state = State::Balansverlies;
        }
        break;

      case State::Fout:
        // Acties van de pijl mf -> Fout. Alle pijlen naar Fout lopen via de
        // merge node mf, en Fout wordt nooit verlaten, dus ze staan hier één keer.
        ESP_LOGE(TAG, "-> Fout: motoren stil tot een herstart (zie de foutmelding hierboven)");
        robotFase.setFase(Fase::FOUT);
        verstuurControl.verstuur();
        foutControl.handelAf();
        // Fout heeft geen uitgaande pijlen: alleen een herstart helpt.
        while (true) {
          vTaskDelay(portMAX_DELAY);
        }
        break;
    }
  }
}
