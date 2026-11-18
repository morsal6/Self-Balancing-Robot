/**
 * @file Mpu6050.hpp
 * @brief Boundary-object en taak voor de MPU6050 (STD Mpu6050).
 */
#pragma once

#include "Meting.hpp"
#include "MetingListener.hpp"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

/**
 * @class Mpu6050
 * @brief Leest na start() iedere REGELPERIODE de MPU6050 uit en geeft de
 *        meting door aan zijn listener.
 *
 * Voorbeeld:
 * @code
 * Mpu6050 mpu6050("Mpu6050", MPU6050_PRIORITEIT, MPU6050_STACK, CORE_REGELING,
 *                 GPIO_NUM_8, GPIO_NUM_9);
 * mpu6050.addListener(&robotControl);
 * mpu6050.start();
 * @endcode
 */
class Mpu6050 {
  /// Toestanden uit het STD van Mpu6050.
  enum class State { Uit, Meten };

  static constexpr EventBits_t eventBitStart = (1 << 0);

private:
  State state = State::Uit;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;

  MetingListener* listener = nullptr;               ///< Ontvanger van de metingen
  i2c_master_dev_handle_t apparaat = nullptr;       ///< I2C-apparaat van de MPU6050

public:
  /**
   * @brief Configureert de I2C-bus en de MPU6050 en start de taak.
   * @param taskName naam van de taak
   * @param taskPriority prioriteit van de taak
   * @param taskStackSizeBytes stackgrootte in bytes
   * @param taskCoreNumber core waarop de taak draait
   * @param sdaPin GPIO van SDA
   * @param sclPin GPIO van SCL
   */
  Mpu6050(const char* taskName, UBaseType_t taskPriority,
          uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
          gpio_num_t sdaPin, gpio_num_t sclPin);

  // STD-interface. Wordt uitgevoerd in de taak van de aanroeper.

  /**
   * @brief Start het periodiek meten.
   */
  void start();

  /**
   * @brief Meldt de listener aan die de metingen ontvangt.
   * @param listener de ontvanger van de metingen
   */
  void addListener(MetingListener* listener);

private:
  /**
   * @brief Leest de versnelling en hoeksnelheid uit de MPU6050.
   * @return de meting; geldig is false als het uitlezen mislukt
   */
  Meting leesSensor();

  static void taskWrapper(void* pParam);
  void main();
};
