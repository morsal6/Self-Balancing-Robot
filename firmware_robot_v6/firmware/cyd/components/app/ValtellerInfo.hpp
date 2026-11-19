/**
 * @file ValtellerInfo.hpp
 * @brief Sessie-id en valteller voor de queue van ScoreControl.
 */
#pragma once

#include <cstdint>

struct ValtellerInfo {
  uint32_t sessieId;
  uint32_t valteller;
};
