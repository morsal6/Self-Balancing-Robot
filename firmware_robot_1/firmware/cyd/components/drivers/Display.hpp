/**
 * @file Display.hpp
 * @brief Boundary-object en taak voor het scherm van de CYD (STD Display).
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
 * @brief Tekent het gezicht (boven) en de score (onder) met drawables.
 *
 * Alleen de taak van Display gebruikt lcd. Andere taken geven via de
 * STD-interface door wat er getekend moet worden; alleen de laatste
 * expressie en de laatste score tellen (queues van lengte 1).
 */
class Display {
  /// Toestanden uit het STD van Display.
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
   * @brief Zet het scherm op (lcd.init(), met de backbuffer), maakt de
   *        synchronisatiemechanismen aan en start de taak.
   *
   * Maak Display als eerste object aan in app_main, zodat de backbuffer
   * nog in het geheugen past.
   * @param taskName naam van de taak
   * @param taskPriority prioriteit van de taak
   * @param taskStackSizeBytes stackgrootte in bytes
   * @param taskCoreNumber core waarop de taak draait
   * @param mosi, sclk, cs, dc, rst, bl pinnen van het ILI9341-scherm
   */
  Display(const char* taskName, UBaseType_t taskPriority,
          uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
          int mosi, int sclk, int cs, int dc, int rst, int bl);

  // STD-interface. Wordt uitgevoerd in de taak van de aanroeper.

  /**
   * @brief Zet het scherm aan.
   */
  void init();

  /**
   * @brief Laat een gezicht tekenen in het bovenste deel van het scherm.
   * @param expressie het te tekenen gezicht
   */
  void tekenGezicht(Expressie expressie);

  /**
   * @brief Laat de score tekenen in het onderste deel van het scherm.
   * @param score de te tonen score
   */
  void tekenScore(uint32_t score);

private:
  /**
   * @brief Tekent een gezicht in de backbuffer.
   * @param expressie het te tekenen gezicht
   */
  void tekenExpressie(Expressie expressie);

  /**
   * @brief Tekent de score met 7-segmentcijfers in de backbuffer.
   * @param score de te tonen score
   */
  void tekenScorebord(uint32_t score);

  /**
   * @brief Tekent één 7-segmentcijfer met rechthoeken.
   * @param cijfer 0..9
   * @param x linkerkant van het cijfer
   * @param y bovenkant van het cijfer
   */
  void tekenCijfer(uint8_t cijfer, int x, int y);

  static void taskWrapper(void* pParam);
  void main();
};
