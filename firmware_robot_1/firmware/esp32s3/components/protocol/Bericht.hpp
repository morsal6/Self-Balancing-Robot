/**
 * @file Bericht.hpp
 * @brief Het ESP-NOW-bericht van de ESP32-S3 naar de CYD.
 *
 * Dit bestand is identiek in de firmware van de ESP32-S3 en van de CYD.
 * Pas je iets aan, doe dat dan in beide projecten.
 */
#pragma once

#include <cstdint>

/**
 * @brief Waar de robot mee bezig is.
 */
enum class Fase : uint8_t {
  KALIBREREN,     ///< Initialiseren en nulpunt bepalen, motoren stil
  BALANCEREN,     ///< De PID-regeling houdt de robot rechtop
  BALANSVERLIES,  ///< Omgevallen, motoren stil, wacht tot hij rechtop staat
  FOUT            ///< Sensor-, initialisatie- of regelingsfout, motoren stil
};

/**
 * @struct Bericht
 * @brief Gegevens die de ESP32-S3 periodiek via ESP-NOW naar de CYD stuurt.
 */
struct Bericht {
  uint32_t sessieId;    ///< Nieuw bij elke start van de ESP32-S3
  uint32_t volgnummer;  ///< Loopt op per bericht
  Fase fase;            ///< Huidige fase van de robot
  float balanswaarde;   ///< Afwijking t.o.v. het nulpunt in graden
  int8_t richting;      ///< Draairichting van de wielen: -1, 0 of 1
  uint32_t valteller;   ///< Aantal balansverliezen sinds de start van de ESP32-S3
};

// Beide subsystemen zijn Xtensa-chips met dezelfde compiler, dus de layout is
// gelijk. Deze controle vangt een onbedoelde wijziging aan één kant op.
static_assert(sizeof(Bericht) == 24, "Bericht moet aan beide kanten even groot zijn");

/// WiFi-kanaal waarop beide subsystemen ESP-NOW gebruiken.
constexpr uint8_t ESPNOW_KANAAL = 1;

/// MAC-adres van de CYD (staat in de monitor van de CYD bij het opstarten).
/// De S3 stuurt hier rechtstreeks naartoe (unicast). Dan bevestigt de CYD
/// elk bericht en ziet de S3 of het aankomt. Alles 0 = broadcast.
constexpr uint8_t CYD_MAC[6] = {0x70, 0x4B, 0xCA, 0x6C, 0xC6, 0x98};
