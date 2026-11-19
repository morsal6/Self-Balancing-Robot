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

/**
 * @class Display
 * @brief Tekent met de drawables uit de les (Circle, Rectangle, Line).
 */
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
  /**
   * @brief Maakt de backbuffer en de taak aan.
   * @param taskName naam van de taak
   * @param taskPriority prioriteit van de taak
   * @param taskStackSizeBytes stackgrootte in bytes
   * @param taskCoreNumber core waarop de taak draait
   * @param mosi SPI MOSI-pin
   * @param sclk SPI klok-pin
   * @param cs chip select-pin
   * @param dc data/command-pin
   * @param rst reset-pin (-1 = geen)
   * @param bl pin van de achtergrondverlichting
   */
  Display(const char* taskName, UBaseType_t taskPriority,
          uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
          int mosi, int sclk, int cs, int dc, int rst, int bl);

  /// @brief Zet het scherm aan (Uit -> Gereed).
  void init();

  /// @brief Tekent een gezicht in de taak van Display.
  void tekenGezicht(Expressie expressie);

  /// @brief Tekent de score in de taak van Display.
  void tekenScore(uint32_t score);

private:
  /// @brief Tekent het gezicht in de backbuffer (alleen het bovenste deel).
  void tekenExpressie(Expressie expressie);

  /// @brief Tekent de score onder het gezicht, zonder voorloopnullen.
  void tekenScorebord(uint32_t score);

  /**
   * @brief Tekent één 7-segment cijfer.
   * @param cijfer 0..9
   * @param x linkerkant
   * @param y bovenkant
   */
  void tekenCijfer(uint8_t cijfer, int x, int y);

  static void taskWrapper(void* pParam);
  void main();
};
