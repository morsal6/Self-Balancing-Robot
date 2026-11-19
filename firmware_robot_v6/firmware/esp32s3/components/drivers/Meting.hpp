/**
 * @file Meting.hpp
 * @brief Eén meting van de MPU6050 (g en graden/s).
 */
#pragma once

struct Meting {
  float ax, ay, az;
  float gx, gy, gz;
  bool geldig;
};
