/**
 * @file MetingListener.hpp
 * @brief Interface voor wie metingen van de Mpu6050 wil krijgen.
 */
#pragma once

#include "Meting.hpp"

class MetingListener {
public:
  virtual void metingGedaan(Meting meting) = 0;

  virtual ~MetingListener() = default;
};
