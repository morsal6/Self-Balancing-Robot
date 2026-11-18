/**
 * @file Ontwerpparameters.hpp
 * @brief Ontwerpparameters en taakparameters van de ESP32-S3.
 *
 * De namen zijn dezelfde als in de STD's. Waarden met (voorlopig) zijn
 * startwaarden; leg de definitieve waarden na het testen vast in het
 * ontwikkeldocument (hoofdstuk Ontwerpparameters).
 */
#pragma once

#include <cstdint>
#include "freertos/FreeRTOS.h"

// ---------------------------------------------------------------------------
// Regeling (UC-S3-01, UC-S3-02, UC-S3-03)
// ---------------------------------------------------------------------------

/// Periode van de regellus in ms (200 Hz; NFR-01 vraagt minimaal 100 Hz). (voorlopig)
constexpr uint32_t REGELPERIODE = 5;

/// REGELPERIODE in seconden, voor het filter en de PID-regelaar.
constexpr float REGELPERIODE_S = REGELPERIODE / 1000.0f;

/// |balanswaarde| in graden waarboven de robot buiten de balans ligt. (voorlopig)
/// Gemeten: omgevallen voorover ca. +80 graden, achterover ca. -53 graden.
/// De grens moet daar ruim onder liggen.
/// Boven ca. 25 graden kunnen de TT-motoren de robot niet meer terughalen;
/// met 30 stopt hij sneller met vol gas rijden na een val.
constexpr float BALANSVERLIESGRENS = 30.0f;

/// Aantal opeenvolgende metingen buiten de grens voor een balansverlies
/// (bevestigingstijd = BEVESTIGINGSAANTAL * REGELPERIODE = 30 ms). Was 100 ms:
/// dan reden de motoren bij een val te lang vol door. (voorlopig)
constexpr uint16_t BEVESTIGINGSAANTAL = 6;

/// |balanswaarde| in graden waaronder de robot weer rechtop staat. (voorlopig)
constexpr float HERSTARTGRENS = 8.0f;

/// Aantal opeenvolgende metingen rechtop en stil voor hervatten (1 s). (voorlopig)
constexpr uint16_t HERSTEL_AANTAL = 200;

/// |gy| in graden per seconde waaronder de robot stil staat. (voorlopig)
/// Gemeten: in de hand stil gehouden schommelt gy tot ca. 4,5 graden/s, en de
/// Mpu6050 trekt geen gyro-offset af. Met 5 zou de kalibratie te vaak opnieuw
/// beginnen.
constexpr float STILGRENS = 10.0f;

/// Aantal opeenvolgende stille metingen voor het nulpunt (2 s). (voorlopig)
constexpr uint16_t STIL_AANTAL = 400;

/// Gyro-offset van gy in graden/s, gemeten in rechtop-stilstand
/// (meetprogramma stap 1: -3,14, ruis 0,69). Wordt in Mpu6050 afgetrokken.
constexpr float GY_OFFSET = -3.14f;

/// Weegfactor van de gyroscoop in het complementaire filter. (voorlopig)
constexpr float FILTER_ALFA = 0.98f;

/// PID-parameters: startwaarden voor het tunen (zie README). (voorlopig)
/// Teken: voorover is een positieve balanswaarde (gemeten op de y-as) en een
/// positieve uitgang rijdt vooruit, dus alle drie positief.
///
/// Geschat uit het 3D-model (Assemgoonbot): zwaartepunt ca. 13 cm boven de
/// as, want scherm en accu's zitten in de kop. Zo'n robot valt met een
/// tijdconstante van ongeveer sqrt(0,13 / 9,81) = 0,12 s.
/// - KP 80: bij 12 graden de volle uitgang (1000). Elke afwijking groter dan
///   0 geeft door MOTOR_STARTWAARDE al minstens 63% PWM.
/// - KD 3: D-tijd KD / KP = ca. 0,04 s, ruim onder de valtijdconstante.
///   Bij 50 graden/s vallen geeft D 150 extra.
/// - KI 0: pas toevoegen als hij rechtop blijft maar langzaam wegrijdt.
///
/// Valtest (meetprogramma stap 3): omega 4,8 en 7,1 /s, gemiddeld ca. 6 /s,
/// dus de robot valt met een tijdconstante 1/omega van ca. 0,17 s.
/// - KP 80 laten staan: genoeg om bij 10 graden ruim boven de startwaarde
///   te komen, verzadigt bij 12,5 graden.
/// - KD 5: D-tijd KD / KP = 0,06 s, ongeveer de helft van 1/omega; dempt
///   het slingeren. Gyroruis (0,7 graden/s) geeft hooguit enkele eenheden.
constexpr float PID_KP = 80.0f;
constexpr float PID_KI = 0.0f;
constexpr float PID_KD = 5.0f;

/// Maximale motoruitgang (FR-02). stuur() verwacht waarden in [-MAX, MAX].
constexpr int16_t MAX_MOTORUITGANG = 1000;

/// Kleinste uitgang (van MAX_MOTORUITGANG) waarbij de motoren vanuit stilstand
/// gaan draaien. Gemeten: PWM 160 van 255 bij 10 kHz, wielen vrij.
/// MotorDriver telt dit op bij elke uitgang die niet 0 is (dode-zonecompensatie).
constexpr int16_t MOTOR_STARTWAARDE = 627;

/// Grootste verandering van de motoruitgang per aanroep (per 5 ms), om
/// stroompieken bij omkeren te beperken. 0 (stoppen) gaat altijd direct.
constexpr int16_t MOTOR_MAX_STAP = 300;

/// Grens voor de integraal van de PID-regelaar (anti-windup), in graad*s.
constexpr float INTEGRAAL_GRENS = 50.0f;

// ---------------------------------------------------------------------------
// Versturen (UC-S3-04)
// ---------------------------------------------------------------------------

/// Verzendperiode van ESP-NOW-berichten in ms (NFR-06: binnen 200 ms zichtbaar). (voorlopig)
constexpr uint32_t VERZENDPERIODE = 50;

// ---------------------------------------------------------------------------
// Taakparameters (taakstructurering)
//
// Core 1 doet alleen de regellus: Mpu6050 meet, RobotControl regelt.
// RobotControl heeft de hoogste prioriteit: zodra er een meting is, wordt
// die direct verwerkt (deadline < REGELPERIODE).
// Core 0 doet alle communicatie: de WiFi-taak van ESP-IDF draait daar al,
// net als esp_timer en de FreeRTOS-timertaak. Een storing in ESP-NOW kan zo
// de regeling niet vertragen (NFR-02).
// ---------------------------------------------------------------------------

constexpr BaseType_t CORE_COMMUNICATIE = 0;
constexpr BaseType_t CORE_REGELING = 1;

constexpr UBaseType_t ROBOTCONTROL_PRIORITEIT = 10;
constexpr UBaseType_t MPU6050_PRIORITEIT = 9;
constexpr UBaseType_t VERSTUURCONTROL_PRIORITEIT = 5;

constexpr uint32_t ROBOTCONTROL_STACK = 4096;
constexpr uint32_t MPU6050_STACK = 3072;
constexpr uint32_t VERSTUURCONTROL_STACK = 3072;
