/**
 * @file Touchscreen.hpp
 * @brief STD Touchscreen: kijkt of het scherm aangeraakt wordt.
 */
#pragma once

#include "TouchListener.hpp"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @class Touchscreen
 * @brief Leest alleen de IRQ-pin van de XPT2046 (laag = aangeraakt), geen positie.
 */
class Touchscreen {
  enum class State { Wachten, Aangeraakt };

private:
  State state = State::Wachten;

  TaskHandle_t taskHandle = nullptr;

  TouchListener* listener = nullptr;
  gpio_num_t irqPin;

public:
  /**
   * @brief Stelt de IRQ-pin in en maakt de taak aan.
   * @param taskName naam van de taak
   * @param taskPriority prioriteit van de taak
   * @param taskStackSizeBytes stackgrootte in bytes
   * @param taskCoreNumber core waarop de taak draait
   * @param irqPin T_IRQ-pin van de XPT2046
   */
  Touchscreen(const char* taskName, UBaseType_t taskPriority,
              uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
              gpio_num_t irqPin);

  /// @brief Zet wie een aanraking te horen krijgt.
  void addListener(TouchListener* listener);

private:
  /// @return true als T_IRQ laag is
  bool isAangeraakt();

  static void taskWrapper(void* pParam);
  void main();
};
