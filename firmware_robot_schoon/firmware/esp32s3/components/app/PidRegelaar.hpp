/**
 * @file PidRegelaar.hpp
 * @brief PID-regelaar voor de balans.
 */
#pragma once

#include <cstdint>

/**
 * @class PidRegelaar
 * @brief Berekent uit de afwijking een motorcommando.
 *
 * Gaat de robot de verkeerde kant op corrigeren, keer dan het teken van
 * PID_KP, PID_KI en PID_KD om (of wissel de motordraden).
 */
class PidRegelaar {
private:
  float kp;
  float ki;
  float kd;
  float integraal = 0.0f;
  float vorigeAfwijking = 0.0f;

public:
  /**
   * @brief Maakt een regelaar met de gegeven parameters.
   * @param kp proportionele versterking
   * @param ki integrerende versterking
   * @param kd differentiërende versterking
   */
  PidRegelaar(float kp, float ki, float kd);

  /**
   * @brief Zet de toestand van de regelaar terug (integraal en vorige afwijking).
   */
  void init();

  /**
   * @brief Berekent het motorcommando. Wordt iedere REGELPERIODE aangeroepen.
   * @param afwijking balanswaarde in graden
   * @return motorcommando, begrensd op [-MAX_MOTORUITGANG, MAX_MOTORUITGANG]
   */
  int16_t bereken(float afwijking);

  /**
   * @brief Zet alleen de integraal op nul.
   */
  void resetIntegraal();
};
