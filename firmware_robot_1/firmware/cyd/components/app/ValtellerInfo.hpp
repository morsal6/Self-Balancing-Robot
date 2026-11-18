/**
 * @file ValtellerInfo.hpp
 * @brief Valteller met sessie-ID, van TonenControl naar ScoreControl.
 */
#pragma once

#include <cstdint>

/**
 * @struct ValtellerInfo
 * @brief Inhoud van de valtellerQueue van ScoreControl.
 */
struct ValtellerInfo {
  uint32_t sessieId;   ///< Sessie-ID van de ESP32-S3
  uint32_t valteller;  ///< Valteller binnen die sessie
};
