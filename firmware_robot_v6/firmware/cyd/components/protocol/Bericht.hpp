/**
 * @file Bericht.hpp
 * @brief Bericht van de S3 naar de CYD. Moet gelijk zijn aan die van de S3.
 */
#pragma once

#include <cstdint>

enum class Fase : uint8_t {
  KALIBREREN,
  BALANCEREN,
  BALANSVERLIES,
  FOUT
};

struct Bericht {
  uint32_t sessieId;
  uint32_t volgnummer;
  Fase fase;
  float balanswaarde;
  int8_t richting;
  uint32_t valteller;
};

static_assert(sizeof(Bericht) == 24, "Bericht moet aan beide kanten even groot zijn");

constexpr uint8_t ESPNOW_KANAAL = 1;

constexpr uint8_t CYD_MAC[6] = {0x70, 0x4B, 0xCA, 0x6C, 0xC6, 0x98};
