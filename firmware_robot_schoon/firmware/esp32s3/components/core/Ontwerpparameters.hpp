/**
 * @file Ontwerpparameters.hpp
 * @brief Ontwerpparameters en taakparameters van de ESP32-S3.
 *
 * De namen zijn dezelfde als in de STD's. Waarden met (voorlopig) zijn
 * startwaarden; leg de definitieve waarden na het testen vast in het
 * ontwikkeldocument (hoofdstuk Ontwerpparameters).
 */
#pragma once

#include "freertos/FreeRTOS.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Regeling (UC-S3-01, UC-S3-02, UC-S3-03)
// ---------------------------------------------------------------------------

/// Periode van de regellus in ms (200 Hz; NFR-01 vraagt minimaal 100 Hz). (voorlopig)
constexpr uint32_t REGELPERIODE = 5;

/// REGELPERIODE in seconden, voor het filter en de PID-regelaar.
constexpr float REGELPERIODE_S = REGELPERIODE / 1000.0f;

/// |balanswaarde| in graden waarboven de robot buiten de balans ligt. (voorlopig)
/// Gemeten: omgevallen voorover ca. +80 graden, achterover ca. -53 graden.
/// Boven ca. 25 graden halen de TT-motoren de robot niet meer terug.
constexpr float BALANSVERLIESGRENS = 30.0f;

/// Aantal opeenvolgende metingen buiten de grens voor een balansverlies
/// (bevestigingstijd = BEVESTIGINGSAANTAL * REGELPERIODE = 30 ms). (voorlopig)
constexpr uint16_t BEVESTIGINGSAANTAL = 6;

/// |balanswaarde| in graden waaronder de robot weer rechtop staat. (voorlopig)
constexpr float HERSTARTGRENS = 8.0f;

/// Aantal opeenvolgende metingen rechtop en stil voor hervatten (1 s). (voorlopig)
constexpr uint16_t HERSTEL_AANTAL = 200;

/// |gy| in graden per seconde waaronder de robot stil staat. (voorlopig)
/// Ruim boven de gyroruis, zodat de robot in de hand stil genoeg is.
constexpr float STILGRENS = 10.0f;

/// Aantal opeenvolgende stille metingen voor het nulpunt (2 s). (voorlopig)
constexpr uint16_t STIL_AANTAL = 400;

/// Gyro-offset van gy in graden/s, gemeten in stilstand (meetprogramma stap 1).
/// Mpu6050 trekt deze waarde af.
constexpr float GY_OFFSET = -3.14f;

/// Weegfactor van de gyroscoop in het complementaire filter. (voorlopig)
constexpr float FILTER_ALFA = 0.98f;

/// PID-parameters (voorlopig). Voorover is een positieve balanswaarde en een
/// positieve uitgang rijdt vooruit, dus alle drie zijn positief.
/// Valtest (meetprogramma stap 3): omega ca. 6 /s, tijdconstante 1/omega ca. 0,17 s.
/// - KP 80: volle uitgang bij 12,5 graden.
/// - KD 5: D-tijd KD / KP = 0,06 s, ruim een derde van 1/omega.
/// - KI 0: pas toevoegen als de robot rechtop blijft maar langzaam wegrijdt.
constexpr float PID_KP = 80.0f;
constexpr float PID_KI = 0.0f;
constexpr float PID_KD = 5.0f;

/// Grens voor de integraal van de PID-regelaar (anti-windup), in graad*s.
constexpr float INTEGRAAL_GRENS = 50.0f;

/// Maximale motoruitgang (FR-02). stuur() verwacht waarden in [-MAX, MAX].
constexpr int16_t MAX_MOTORUITGANG = 1000;

/// Kleinste uitgang (van MAX_MOTORUITGANG) waarbij de motoren vanuit stilstand
/// draaien. Gemeten: PWM 160 van 255 bij 10 kHz, wielen vrij.
constexpr int16_t MOTOR_STARTWAARDE = 627;

/// Grootste verandering van de motoruitgang per aanroep (dus per 5 ms).
/// Beperkt de stroompiek bij omkeren van de draairichting.
constexpr int16_t MOTOR_MAX_STAP = 300;

// ---------------------------------------------------------------------------
// Versturen (UC-S3-04)
// ---------------------------------------------------------------------------

/// Verzendperiode van ESP-NOW-berichten in ms (NFR-06: binnen 200 ms zichtbaar). (voorlopig)
constexpr uint32_t VERZENDPERIODE = 50;

// ---------------------------------------------------------------------------
// Taakparameters (taakstructurering)
//
// Core 1 doet alleen de regellus: Mpu6050 meet, RobotControl regelt.
// RobotControl heeft de hoogste prioriteit, zodat een meting direct verwerkt
// wordt (deadline < REGELPERIODE).
// Core 0 doet de communicatie, samen met de WiFi-taak, esp_timer en de
// FreeRTOS-timertaak van ESP-IDF.
// ---------------------------------------------------------------------------

constexpr BaseType_t CORE_COMMUNICATIE = 0;
constexpr BaseType_t CORE_REGELING = 1;

constexpr UBaseType_t ROBOTCONTROL_PRIORITEIT = 10;
constexpr UBaseType_t MPU6050_PRIORITEIT = 9;
constexpr UBaseType_t VERSTUURCONTROL_PRIORITEIT = 5;

constexpr uint32_t ROBOTCONTROL_STACK = 4096;
constexpr uint32_t MPU6050_STACK = 3072;
constexpr uint32_t VERSTUURCONTROL_STACK = 3072;
