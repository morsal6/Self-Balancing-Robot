/**
 * @file Meting.hpp
 * @brief Eén meting van de MPU6050.
 */
#pragma once

/**
 * @struct Meting
 * @brief Versnelling en hoeksnelheid van de MPU6050.
 */
struct Meting {
  float ax, ay, az;  ///< Versnelling in g
  float gx, gy, gz;  ///< Hoeksnelheid in graden per seconde
  bool geldig;       ///< false als de sensor niet uitgelezen kon worden
};
