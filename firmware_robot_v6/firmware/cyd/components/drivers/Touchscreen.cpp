/**
 * @file Touchscreen.cpp
 * @brief Implementatie van Touchscreen.
 */
#include "Touchscreen.hpp"

#include "Ontwerpparameters.hpp"

Touchscreen::Touchscreen(const char* taskName, UBaseType_t taskPriority,
                         uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                         gpio_num_t irqPin)
    : irqPin(irqPin) {
  gpio_config_t config = {};
  config.pin_bit_mask = (1ULL << irqPin);
  config.mode = GPIO_MODE_INPUT;
  config.pull_up_en = GPIO_PULLUP_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;
  ESP_ERROR_CHECK(gpio_config(&config));

  const BaseType_t gestart = xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes,
                                                     this, taskPriority, &taskHandle,
                                                     taskCoreNumber);
  configASSERT(gestart == pdPASS);
}

void Touchscreen::addListener(TouchListener* listener) {
  this->listener = listener;
}

bool Touchscreen::isAangeraakt() {
  return gpio_get_level(irqPin) == 0;  // T_IRQ is laag zolang je het scherm aanraakt
}

void Touchscreen::taskWrapper(void* pParam) {
  static_cast<Touchscreen*>(pParam)->main();
}

void Touchscreen::main() {
  while (true) {
    switch (state) {
      case State::Wachten:
        vTaskDelay(pdMS_TO_TICKS(TOUCH_PERIODE));

        if (isAangeraakt()) {
          if (listener != nullptr) {
            listener->aangeraakt();
          }
          state = State::Aangeraakt;
        } else {
          state = State::Wachten;
        }
        break;

      case State::Aangeraakt:
        vTaskDelay(pdMS_TO_TICKS(TOUCH_PERIODE));

        if (!isAangeraakt()) {
          state = State::Wachten;
        } else {
          state = State::Aangeraakt;
        }
        break;
    }
  }
}
