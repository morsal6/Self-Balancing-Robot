/**
 * @file Touchscreen.hpp
 * @brief Boundary-object en taak voor het touchscreen (STD Touchscreen).
 */
#pragma once

#include "TouchListener.hpp"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @class Touchscreen
 * @brief Kijkt iedere TOUCH_PERIODE of het scherm wordt aangeraakt en meldt
 *        een nieuwe aanraking één keer aan zijn listener.
 *
 * Gebruikt alleen de T_IRQ-uitgang van de XPT2046 (GPIO36 op de CYD). Die is
 * laag zolang het scherm wordt aangeraakt. De positie is niet nodig: een tik
 * waar dan ook start een nieuw spel.
 */
class Touchscreen {
  /// Toestanden uit het STD van Touchscreen.
  enum class State { Wachten, Aangeraakt };

private:
  State state = State::Wachten;

  TaskHandle_t taskHandle = nullptr;

  TouchListener* listener = nullptr;
  gpio_num_t irqPin;  ///< T_IRQ van de XPT2046

public:
  /**
   * @brief Configureert de T_IRQ-pin en start de taak.
   * @param taskName naam van de taak
   * @param taskPriority prioriteit van de taak
   * @param taskStackSizeBytes stackgrootte in bytes
   * @param taskCoreNumber core waarop de taak draait
   * @param irqPin GPIO van T_IRQ
   */
  Touchscreen(const char* taskName, UBaseType_t taskPriority,
              uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
              gpio_num_t irqPin);

  /**
   * @brief Meldt de listener aan die de aanrakingen ontvangt.
   * @param listener de ontvanger van de aanrakingen
   */
  void addListener(TouchListener* listener);

private:
  /**
   * @brief Kijkt of het scherm nu wordt aangeraakt.
   * @return true als het scherm wordt aangeraakt
   */
  bool isAangeraakt();

  static void taskWrapper(void* pParam);
  void main();
};
