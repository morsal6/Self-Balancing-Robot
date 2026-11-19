/**
 * @file ValtellerInfo.hpp
 * @brief Sessie-id en valteller voor de queue van ScoreControl.
 */
#pragma once

#include <cstdint>

/**
 * @class ValtellerInfo
 * @brief Sessie-id en valteller uit een bericht.
 */
struct ValtellerInfo {
  uint32_t sessieId;
  uint32_t valteller;
};
