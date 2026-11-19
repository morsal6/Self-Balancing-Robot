/**
 * @file Meting.hpp
 * @brief Eén meting van de MPU6050 (g en graden/s).
 */
#pragma once

/**
 * @class Meting
 * @brief Versnelling in g, draaisnelheid in graden/s.
 */
struct Meting {
  float ax, ay, az;
  float gx, gy, gz;
  bool geldig;
};
