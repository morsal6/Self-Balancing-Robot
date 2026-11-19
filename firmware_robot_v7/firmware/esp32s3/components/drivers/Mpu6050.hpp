/**
 * @file Mpu6050.hpp
 * @brief STD Mpu6050: leest elke REGELPERIODE de sensor uit.
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
 * @brief Leest de MPU6050 via I2C uit en geeft elke meting aan de listener.
 */
class Mpu6050 {
  enum class State { Uit, Meten };

  static constexpr EventBits_t eventBitStart = (1 << 0);

private:
  State state = State::Uit;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;

  MetingListener* listener = nullptr;
  i2c_master_dev_handle_t apparaat = nullptr;

public:
  /**
   * @brief Stelt de I2C-bus en de sensor in en maakt de taak aan.
   * @param taskName naam van de taak
   * @param taskPriority prioriteit van de taak
   * @param taskStackSizeBytes stackgrootte in bytes
   * @param taskCoreNumber core waarop de taak draait
   * @param sdaPin SDA-pin
   * @param sclPin SCL-pin
   */
  Mpu6050(const char* taskName, UBaseType_t taskPriority,
          uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
          gpio_num_t sdaPin, gpio_num_t sclPin);

  /// @brief Begint met meten (Uit -> Meten).
  void start();

  /// @brief Zet wie de metingen krijgt.
  void addListener(MetingListener* listener);

private:
  /**
   * @brief Leest alle 14 bytes in één keer (maximaal LEESPOGINGEN keer).
   * @return meting met geldig = false als het lezen mislukt
   */
  Meting leesSensor();

  static void taskWrapper(void* pParam);
  void main();
};
