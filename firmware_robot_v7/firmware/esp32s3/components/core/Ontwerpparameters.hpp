/**
 * @file Ontwerpparameters.hpp
 * @brief Alle instelbare waarden van de robot.
 */
#pragma once

#include "freertos/FreeRTOS.h"

#include <cstdint>

constexpr uint32_t REGELPERIODE = 5;  // ms

constexpr float REGELPERIODE_S = REGELPERIODE / 1000.0f;

constexpr float BALANSVERLIESGRENS = 30.0f;  // graden, daarboven is hij gevallen

constexpr uint16_t BEVESTIGINGSAANTAL = 6;  // zoveel metingen achter elkaar boven de grens

constexpr float HERSTARTGRENS = 8.0f;  // graden, binnen deze hoek telt hij als rechtop

constexpr uint16_t HERSTEL_AANTAL = 200;  // 1 s rechtop en stil voor hij weer begint

constexpr float STILGRENS = 10.0f;  // graden/s

constexpr uint16_t STIL_AANTAL = 400;  // 2 s stil voor het nulpunt

constexpr float GY_OFFSET = -3.14f;  // gemeten met test_s3_meting stap 1

constexpr float FILTER_ALFA = 0.995f;  // bij 0.98 telde het optrekken te veel mee

// Getest op de robot, zie README (PID afstellen).
constexpr float PID_KP = 80.0f;
constexpr float PID_KI = 200.0f;
constexpr float PID_KD = 2.0f;

constexpr float INTEGRAAL_GRENS = 5.0f;  // anti-windup

// Snelheidsterugkoppeling: zonder dit rijdt hij langzaam weg en valt.
// doelhoek = -(SNELHEID_KP * snelheid + SNELHEID_KI * snelheidIntegraal)
constexpr float SNELHEID_KP = 5.0f;
constexpr float SNELHEID_KI = 2.0f;

constexpr float SNELHEID_FILTERTIJD = 0.3f;  // s

constexpr float SNELHEID_INTEGRAAL_GRENS = 5.0f;  // max 10 graden extra doelhoek

constexpr int16_t MAX_MOTORUITGANG = 1000;

constexpr int16_t MOTOR_STARTWAARDE = 627;  // daaronder draaien de motoren niet (gemeten)

constexpr int16_t MOTOR_MAX_STAP = 300;  // per aanroep, tegen stroompieken

constexpr uint32_t VERZENDPERIODE = 50;  // ms

// WiFi draait op core 0, de regeling op core 1.
constexpr BaseType_t CORE_COMMUNICATIE = 0;
constexpr BaseType_t CORE_REGELING = 1;

constexpr UBaseType_t ROBOTCONTROL_PRIORITEIT = 10;
constexpr UBaseType_t MPU6050_PRIORITEIT = 9;
constexpr UBaseType_t VERSTUURCONTROL_PRIORITEIT = 5;

constexpr uint32_t ROBOTCONTROL_STACK = 4096;
constexpr uint32_t MPU6050_STACK = 3072;
constexpr uint32_t VERSTUURCONTROL_STACK = 3072;
