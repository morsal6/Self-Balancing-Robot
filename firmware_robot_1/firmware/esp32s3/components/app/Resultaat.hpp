/**
 * @file Resultaat.hpp
 * @brief Resultaat van verwerkMeting() van de passieve controls.
 */
#pragma once

/**
 * @brief Wat RobotControl na een verwerkte meting moet doen.
 */
enum class Resultaat {
  BEZIG,          ///< De use case loopt nog
  KLAAR,          ///< De use case is klaar
  BALANSVERLIES,  ///< Er is een balansverlies vastgesteld
  FOUT            ///< Er is een fout opgetreden
};
