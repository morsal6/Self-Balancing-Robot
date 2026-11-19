/**
 * @file Touchscreen.hpp
 * @brief STD Touchscreen: kijkt of het scherm aangeraakt wordt.
 */
#pragma once

#include "TouchListener.hpp"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/// @brief Leest alleen de IRQ-pin van de XPT2046 (laag = aangeraakt), geen positie.
class Touchscreen {
  enum class State { Wachten, Aangeraakt };

private:
  State state = State::Wachten;

  TaskHandle_t taskHandle = nullptr;

  TouchListener* listener = nullptr;
  gpio_num_t irqPin;

public:
  Touchscreen(const char* taskName, UBaseType_t taskPriority,
              uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
              gpio_num_t irqPin);

  void addListener(TouchListener* listener);

private:
  bool isAangeraakt();

  static void taskWrapper(void* pParam);
  void main();
};
