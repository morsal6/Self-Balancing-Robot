/**
 * @file Touchscreen.cpp
 * @brief Implementatie van het STD van Touchscreen.
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
  config.pull_up_en = GPIO_PULLUP_DISABLE;  // GPIO36 heeft geen interne pull-up
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
  return gpio_get_level(irqPin) == 0;
}

void Touchscreen::taskWrapper(void* pParam) {
  static_cast<Touchscreen*>(pParam)->main();
}

void Touchscreen::main() {
  while (true) {
    switch (state) {
      case State::Wachten:
        // after(TOUCH_PERIODE): alleen op de tijd wachten, dus vTaskDelay.
        vTaskDelay(pdMS_TO_TICKS(TOUCH_PERIODE));
        // d1
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
        // d2
        if (!isAangeraakt()) {
          state = State::Wachten;
        } else {
          state = State::Aangeraakt;
        }
        break;
    }
  }
}
