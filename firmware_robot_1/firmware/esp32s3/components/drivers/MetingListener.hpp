/**
 * @file MetingListener.hpp
 * @brief Interface voor objecten die metingen van de Mpu6050 ontvangen.
 */
#pragma once

#include "Meting.hpp"

/**
 * @class MetingListener
 * @brief Listener-interface van de Mpu6050.
 */
class MetingListener {
public:
  /**
   * @brief Wordt aangeroepen als er een nieuwe meting is.
   * @param meting de nieuwe meting
   */
  virtual void metingGedaan(Meting meting) = 0;

  virtual ~MetingListener() = default;
};
