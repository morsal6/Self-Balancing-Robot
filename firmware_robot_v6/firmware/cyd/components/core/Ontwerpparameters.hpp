/**
 * @file Ontwerpparameters.hpp
 * @brief Instelbare waarden van de CYD.
 */
#pragma once

#include "freertos/FreeRTOS.h"

#include <cstdint>

constexpr uint32_t COMM_TIMEOUT = 500;  // ms zonder geldig bericht = verbinding weg

constexpr float SCHRIKGRENS = 12.0f;  // graden, daarboven schrikt het gezicht

constexpr float OOG_SCHAAL = 1.5f;  // pixels per graad

constexpr int8_t RICHTING_OOGX = 4;

constexpr int8_t MAX_OOGX = 18;

constexpr uint32_t MAX_SCORE = 999;

constexpr uint32_t TOUCH_PERIODE = 30;  // ms

constexpr uint16_t ACHTERGROND = 0x0000;

constexpr int GEZICHT_HOOGTE = 180;  // daaronder staat de score

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
