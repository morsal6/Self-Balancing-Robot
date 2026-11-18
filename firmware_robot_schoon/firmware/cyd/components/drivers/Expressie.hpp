/**
 * @file Expressie.hpp
 * @brief Een gezichtsexpressie voor het Display.
 */
#pragma once

#include <cstdint>

/**
 * @brief Soorten gezichten (UC-CYD-01 stap 5 en UC-CYD-03).
 */
enum class ExpressieSoort : uint8_t {
  OPSTARTEN,           ///< KALIBREREN: slaperige ogen
  BALANCEREN,          ///< BALANCEREN: ogen volgen de kanteling
  SCHRIKKEN,           ///< BALANCEREN met grote balanswaarde: grote ogen
  GEVALLEN,            ///< BALANSVERLIES: kruisjes als ogen
  FOUT,                ///< FOUT: rode ogen
  VERBINDING_VERLOREN  ///< Geen geldig bericht binnen COMM_TIMEOUT
};

/**
 * @struct Expressie
 * @brief Wat het Display moet tekenen.
 */
struct Expressie {
  ExpressieSoort soort;  ///< Soort gezicht
  int8_t oogX;           ///< Horizontale verschuiving van de pupillen in pixels
};
