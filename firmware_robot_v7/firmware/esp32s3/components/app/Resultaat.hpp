/**
 * @file Resultaat.hpp
 * @brief Wat een verwerkMeting() teruggeeft aan RobotControl.
 */
#pragma once

/// @brief Resultaat van verwerkMeting(): bepaalt de volgende toestand.
enum class Resultaat {
  BEZIG,
  KLAAR,
  BALANSVERLIES,
  FOUT
};
