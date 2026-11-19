/**
 * @file RobotControl.hpp
 * @brief STD RobotControl: de hoofdtaak van de robot.
 */
#pragma once

#include "BalansControl.hpp"
#include "BalansverliesControl.hpp"
#include "FoutControl.hpp"
#include "InitControl.hpp"
#include "Meting.hpp"
#include "MetingListener.hpp"
#include "Mpu6050.hpp"
#include "RobotFase.hpp"
#include "VerstuurControl.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"

/// @brief Krijgt elke meting van de Mpu6050 en geeft die aan de control van de huidige toestand.
class RobotControl : public MetingListener {
  enum class State { Initialiseren, Balanceren, Balansverlies, Fout };

  static constexpr EventBits_t eventBitMeting = (1 << 0);

private:
  State state = State::Initialiseren;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;
  QueueHandle_t metingQueue = nullptr;

  RobotFase& robotFase;
  VerstuurControl& verstuurControl;
  InitControl& initControl;
  BalansControl& balansControl;
  BalansverliesControl& balansverliesControl;
  FoutControl& foutControl;

public:
  RobotControl(const char* taskName, UBaseType_t taskPriority,
               uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
               RobotFase& robotFase, VerstuurControl& verstuurControl,
               InitControl& initControl, BalansControl& balansControl,
               BalansverliesControl& balansverliesControl, FoutControl& foutControl,
               Mpu6050& mpu6050);

  void metingGedaan(Meting meting) override;

private:
  static void taskWrapper(void* pParam);
  void main();
};
