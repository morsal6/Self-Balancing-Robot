/**
 * @file PidRegelaar.hpp
 * @brief PID-regelaar voor de balans, met snelheidsterugkoppeling.
 */
#pragma once

#include <cmath>
#include <cstdint>

/**
 * @class PidRegelaar
 * @brief Rekent de balanswaarde om naar een motorcommando.
 *
 * Houd kp, ki en kd positief. Draait een motor de verkeerde kant op, wissel
 * dan de motordraden.
 */
class PidRegelaar {
private:
  float kp;
  float ki;
  float kd;
  float integraal = 0.0f;
  float vorigeAfwijking = NAN;  // NAN = nog geen vorige waarde
  float snelheid = 0.0f;  // gefilterd motorcommando, -1..1
  float snelheidIntegraal = 0.0f;  // ongeveer de afgelegde weg

public:
  /**
   * @brief Maakt de regelaar.
   * @param kp proportionele versterking
   * @param ki integrerende versterking
   * @param kd differentiërende versterking
   */
  PidRegelaar(float kp, float ki, float kd);

  /// @brief Zet integraal, vorige afwijking en snelheid terug.
  void init();

  /**
   * @brief Berekent het motorcommando.
   * @param balanswaarde kanteling in graden
   * @return 0 of tussen MOTOR_STARTWAARDE en MAX_MOTORUITGANG (met teken)
   */
  int16_t bereken(float balanswaarde);

  /// @brief Zet alleen de integraal op 0.
  void resetIntegraal();
};
