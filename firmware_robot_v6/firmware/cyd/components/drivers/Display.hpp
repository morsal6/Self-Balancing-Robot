/**
 * @file Display.hpp
 * @brief STD Display: tekent het gezicht en de score.
 */
#pragma once

#include "Expressie.hpp"
#include "Ili9341Display.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <cstdint>

/// @brief Tekent met de drawables uit de les (Circle, Rectangle, Line).
class Display {
  enum class State { Uit, Gereed };

  static constexpr EventBits_t eventBitInit = (1 << 0);
  static constexpr EventBits_t eventBitGezicht = (1 << 1);
  static constexpr EventBits_t eventBitScore = (1 << 2);

private:
  State state = State::Uit;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;
  QueueHandle_t expressieQueue = nullptr;
  QueueHandle_t scoreQueue = nullptr;

  Ili9341Display lcd;

public:
  Display(const char* taskName, UBaseType_t taskPriority,
          uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
          int mosi, int sclk, int cs, int dc, int rst, int bl);

  void init();

  void tekenGezicht(Expressie expressie);

  void tekenScore(uint32_t score);

private:
  void tekenExpressie(Expressie expressie);

  void tekenScorebord(uint32_t score);

  void tekenCijfer(uint8_t cijfer, int x, int y);

  static void taskWrapper(void* pParam);
  void main();
};
