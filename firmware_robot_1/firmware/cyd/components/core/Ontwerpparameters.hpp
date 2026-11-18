/**
 * @file Ontwerpparameters.hpp
 * @brief Ontwerpparameters en taakparameters van de CYD.
 *
 * De namen zijn dezelfde als in de STD's. Waarden met (voorlopig) zijn
 * startwaarden; leg de definitieve waarden na het testen vast in het
 * ontwikkeldocument (hoofdstuk Ontwerpparameters).
 */
#pragma once

#include <cstdint>
#include "freertos/FreeRTOS.h"

// ---------------------------------------------------------------------------
// Tonen (UC-CYD-01, UC-CYD-03)
// ---------------------------------------------------------------------------

/// Tijd in ms zonder geldig bericht waarna communicatieverlies getoond wordt. (voorlopig)
constexpr uint32_t COMM_TIMEOUT = 500;

/// |balanswaarde| in graden waarboven het gezicht schrikt. (voorlopig)
constexpr float SCHRIKGRENS = 12.0f;

/// Verschuiving van de pupillen in pixels per graad balanswaarde. (voorlopig)
constexpr float OOG_SCHAAL = 1.5f;

/// Extra verschuiving van de pupillen in pixels in de rijrichting. (voorlopig)
constexpr int8_t RICHTING_OOGX = 4;

/// Maximale verschuiving van de pupillen in pixels (expressiebereik).
constexpr int8_t MAX_OOGX = 18;

// ---------------------------------------------------------------------------
// Score (UC-CYD-02)
// ---------------------------------------------------------------------------

/// Hoogste score; er zijn drie cijfers op het scherm.
constexpr uint32_t MAX_SCORE = 999;

/// Periode in ms waarmee het touchscreen wordt bekeken. (voorlopig)
constexpr uint32_t TOUCH_PERIODE = 30;

// ---------------------------------------------------------------------------
// Scherm
// ---------------------------------------------------------------------------

/// Achtergrondkleur (zwart is in elke kleurvolgorde 0x0000).
constexpr uint16_t ACHTERGROND = 0x0000;

/// Het gezicht staat in het bovenste deel, de score in de rest (FR-10).
constexpr int GEZICHT_HOOGTE = 180;

// ---------------------------------------------------------------------------
// Taakparameters (taakstructurering)
//
// Core 1 tekent alleen: Display vult de backbuffer en stuurt die met
// present() in één keer naar het scherm (ruim 40 ms per frame).
// Core 0 doet de rest: de WiFi-taak (ontvangst via ESP-NOW) draait daar al,
// net als de FreeRTOS-timertaak van commTimer.
// ---------------------------------------------------------------------------

constexpr BaseType_t CORE_COMMUNICATIE = 0;
constexpr BaseType_t CORE_TEKENEN = 1;

constexpr UBaseType_t TONENCONTROL_PRIORITEIT = 5;
constexpr UBaseType_t SCORECONTROL_PRIORITEIT = 4;
constexpr UBaseType_t TOUCHSCREEN_PRIORITEIT = 3;
constexpr UBaseType_t DISPLAY_PRIORITEIT = 5;

constexpr uint32_t TONENCONTROL_STACK = 4096;
constexpr uint32_t SCORECONTROL_STACK = 3072;
constexpr uint32_t TOUCHSCREEN_STACK = 2048;
constexpr uint32_t DISPLAY_STACK = 4096;
