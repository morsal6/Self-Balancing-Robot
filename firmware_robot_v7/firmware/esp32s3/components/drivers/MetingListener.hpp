/**
 * @file MetingListener.hpp
 * @brief Interface voor wie metingen van de Mpu6050 wil krijgen.
 */
#pragma once

#include "Meting.hpp"

/**
 * @class MetingListener
 * @brief Interface voor wie metingen van de Mpu6050 wil krijgen.
 */
class MetingListener {
public:
  /// @brief Wordt elke REGELPERIODE aangeroepen vanuit de taak van Mpu6050.
  virtual void metingGedaan(Meting meting) = 0;

  virtual ~MetingListener() = default;
};
