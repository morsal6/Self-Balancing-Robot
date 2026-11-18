/**
 * @file RobotControl.hpp
 * @brief Control-object en taak van UC-S3-00 Robot besturen (STD RobotControl).
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

/**
 * @class RobotControl
 * @brief "Dirigent" van UC-S3-00. Voert achtereenvolgens InitControl,
 *        BalansControl en BalansverliesControl uit, en bij een fout FoutControl.
 *
 * RobotControl is de enige listener van de Mpu6050. Iedere meting wordt
 * doorgegeven aan de control van de huidige toestand.
 */
class RobotControl : public MetingListener {
  /// Toestanden uit het STD van RobotControl.
  enum class State { Initialiseren, Balanceren, Balansverlies, Fout };

  static constexpr EventBits_t eventBitMeting = (1 << 0);

private:
  State state = State::Initialiseren;

  TaskHandle_t taskHandle = nullptr;
  EventGroupHandle_t eventGroup = nullptr;
  QueueHandle_t metingQueue = nullptr;  ///< Lengte 1: alleen de laatste meting telt

  RobotFase& robotFase;
  VerstuurControl& verstuurControl;
  InitControl& initControl;
  BalansControl& balansControl;
  BalansverliesControl& balansverliesControl;
  FoutControl& foutControl;

public:
  /**
   * @brief Maakt de synchronisatiemechanismen aan, meldt zich als listener aan
   *        bij de Mpu6050 en start de taak.
   *
   * mpu6050 is alleen een constructorparameter (geen member): RobotControl
   * heeft hem alleen nodig om zich als listener aan te melden.
   */
  RobotControl(const char* taskName, UBaseType_t taskPriority,
               uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
               RobotFase& robotFase, VerstuurControl& verstuurControl,
               InitControl& initControl, BalansControl& balansControl,
               BalansverliesControl& balansverliesControl, FoutControl& foutControl,
               Mpu6050& mpu6050);

  // STD-interface. Wordt uitgevoerd in de taak van de aanroeper (Mpu6050).

  /**
   * @brief Ontvangt een nieuwe meting van de Mpu6050.
   * @param meting de nieuwe meting
   */
  void metingGedaan(Meting meting) override;

private:
  static void taskWrapper(void* pParam);
  void main();
};
