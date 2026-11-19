/**
 * @file PidRegelaar.hpp
 * @brief PID-regelaar voor de balans, met snelheidsterugkoppeling.
 */
#pragma once

#include <cmath>
#include <cstdint>

/**
 * @class PidRegelaar
 * @brief Berekent uit de balanswaarde het motorcommando.
 *
 * Elke REGELPERIODE:
 * 1. De snelheidsterugkoppeling kiest een doelhoek (zie SNELHEID_KP in
 *    Ontwerpparameters.hpp). Zo rijdt de robot niet steeds harder één kant op.
 * 2. De PID-regelaar stuurt op de afwijking ten opzichte van die doelhoek.
 * 3. Dode-zonecompensatie: een uitgang die niet 0 is begint bij
 *    MOTOR_STARTWAARDE. Dat gebeurt hier en niet in MotorDriver, omdat de
 *    snelheid wordt geschat uit dit echte motorcommando.
 * 4. De geschatte snelheid wordt bijgewerkt voor de volgende keer.
 *
 * Gaat de robot de verkeerde kant op corrigeren, wissel dan de motordraden
 * (of de pinnen A en B in main.cpp). Keer niet het teken van de versterkingen
 * om, want dan werkt de snelheidsterugkoppeling de verkeerde kant op.
 */
class PidRegelaar {
private:
  float kp;
  float ki;
  float kd;
  float integraal = 0.0f;
  float vorigeAfwijking = NAN;     ///< NAN: nog geen vorige afwijking (na init())
  float snelheid = 0.0f;           ///< Geschatte snelheid, -1 (vol achteruit) .. 1 (vol vooruit)
  float snelheidIntegraal = 0.0f;  ///< Integraal van de snelheid: maat voor de afgelegde weg

public:
  /**
   * @brief Maakt een regelaar met de gegeven parameters.
   * @param kp proportionele versterking
   * @param ki integrerende versterking
   * @param kd differentiërende versterking
   */
  PidRegelaar(float kp, float ki, float kd);

  /**
   * @brief Zet de toestand van de regelaar terug: integraal 0, geen vorige
   *        afwijking, snelheid 0 en snelheidsintegraal 0.
   */
  void init();

  /**
   * @brief Berekent het motorcommando. Wordt iedere REGELPERIODE aangeroepen.
   * @param balanswaarde kanteling in graden ten opzichte van het nulpunt
   * @return motorcommando: 0, of tussen MOTOR_STARTWAARDE en MAX_MOTORUITGANG
   *         (met teken)
   */
  int16_t bereken(float balanswaarde);

  /**
   * @brief Zet alleen de integraal op nul.
   */
  void resetIntegraal();
};
