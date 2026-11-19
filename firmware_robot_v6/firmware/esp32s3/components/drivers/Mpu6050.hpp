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
  Mpu6050(const char* taskName, UBaseType_t taskPriority,
          uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
          gpio_num_t sdaPin, gpio_num_t sclPin);

  void start();

  void addListener(MetingListener* listener);

private:
  Meting leesSensor();

  static void taskWrapper(void* pParam);
  void main();
};
