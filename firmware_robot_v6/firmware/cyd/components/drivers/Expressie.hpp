/**
 * @file Expressie.hpp
 * @brief Welk gezicht er getekend moet worden.
 */
#pragma once

#include <cstdint>

enum class ExpressieSoort : uint8_t {
  OPSTARTEN,
  BALANCEREN,
  SCHRIKKEN,
  GEVALLEN,
  FOUT,
  VERBINDING_VERLOREN
};

struct Expressie {
  ExpressieSoort soort;
  int8_t oogX;  // verschuiving van de pupillen in pixels
};
