/**
 * @file PidRegelaar.cpp
 * @brief Implementatie van PidRegelaar.
 */
#include "PidRegelaar.hpp"

#include "Ontwerpparameters.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

PidRegelaar::PidRegelaar(float kp, float ki, float kd) : kp(kp), ki(ki), kd(kd) {}

void PidRegelaar::init() {
  integraal = 0.0f;
  vorigeAfwijking = NAN;
  snelheid = 0.0f;
  snelheidIntegraal = 0.0f;
}

int16_t PidRegelaar::bereken(float balanswaarde) {
  // 1. Doelhoek: rijdt de robot vooruit (snelheid > 0), dan moet hij iets
  //    achterover hellen om af te remmen, en andersom.
  const float doelhoek = -(SNELHEID_KP * snelheid + SNELHEID_KI * snelheidIntegraal);

  // 2. PID op de afwijking ten opzichte van de doelhoek.
  const float afwijking = balanswaarde - doelhoek;
  // Anti-windup: de integraal wordt begrensd.
  integraal = std::clamp(integraal + afwijking * REGELPERIODE_S, -INTEGRAAL_GRENS, INTEGRAAL_GRENS);
  // Direct na init() is er geen vorige afwijking. Dan geen D-actie: anders
  // geeft de eerste meting na het (her)starten een piek op de motoren.
  const float differentiaal =
      std::isnan(vorigeAfwijking) ? 0.0f : (afwijking - vorigeAfwijking) / REGELPERIODE_S;
  vorigeAfwijking = afwijking;
  const float uitgang = std::clamp(kp * afwijking + ki * integraal + kd * differentiaal,
                                   static_cast<float>(-MAX_MOTORUITGANG),
                                   static_cast<float>(MAX_MOTORUITGANG));

  // 3. Dode-zonecompensatie: onder MOTOR_STARTWAARDE draaien de motoren niet,
  //    dus elke uitgang die niet 0 is begint daar. 0 blijft 0 (stil).
  int32_t grootte = std::abs(static_cast<int32_t>(uitgang));
  if (grootte > 0) {
    grootte = MOTOR_STARTWAARDE + grootte * (MAX_MOTORUITGANG - MOTOR_STARTWAARDE) / MAX_MOTORUITGANG;
  }
  const int16_t commando = static_cast<int16_t>((uitgang < 0) ? -grootte : grootte);

  // 4. Snelheid schatten voor de volgende keer: het commando door een
  //    laagdoorlaatfilter, want de wielen volgen het commando met vertraging.
  snelheid += (static_cast<float>(commando) / MAX_MOTORUITGANG - snelheid) *
              (REGELPERIODE_S / SNELHEID_FILTERTIJD);
  snelheidIntegraal = std::clamp(snelheidIntegraal + snelheid * REGELPERIODE_S,
                                 -SNELHEID_INTEGRAAL_GRENS, SNELHEID_INTEGRAAL_GRENS);
  return commando;
}

void PidRegelaar::resetIntegraal() {
  integraal = 0.0f;
}
