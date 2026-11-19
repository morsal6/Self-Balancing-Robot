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
  // Rijdt hij vooruit, dan moet hij iets achterover hellen om af te remmen.
  const float doelhoek = -(SNELHEID_KP * snelheid + SNELHEID_KI * snelheidIntegraal);

  const float afwijking = balanswaarde - doelhoek;

  integraal = std::clamp(integraal + afwijking * REGELPERIODE_S, -INTEGRAAL_GRENS, INTEGRAAL_GRENS);

  // Geen D-actie bij de eerste meting, anders krijg je een piek.
  const float differentiaal =
      std::isnan(vorigeAfwijking) ? 0.0f : (afwijking - vorigeAfwijking) / REGELPERIODE_S;
  vorigeAfwijking = afwijking;
  const float uitgang = std::clamp(kp * afwijking + ki * integraal + kd * differentiaal,
                                   static_cast<float>(-MAX_MOTORUITGANG),
                                   static_cast<float>(MAX_MOTORUITGANG));

  // Onder MOTOR_STARTWAARDE draaien de motoren niet, dus daar begint de uitgang.
  int32_t grootte = std::abs(static_cast<int32_t>(uitgang));
  if (grootte > 0) {
    grootte = MOTOR_STARTWAARDE + grootte * (MAX_MOTORUITGANG - MOTOR_STARTWAARDE) / MAX_MOTORUITGANG;
  }
  const int16_t commando = static_cast<int16_t>((uitgang < 0) ? -grootte : grootte);

  // Snelheid schatten voor de volgende keer (geen encoders).
  snelheid += (static_cast<float>(commando) / MAX_MOTORUITGANG - snelheid) *
              (REGELPERIODE_S / SNELHEID_FILTERTIJD);
  snelheidIntegraal = std::clamp(snelheidIntegraal + snelheid * REGELPERIODE_S,
                                 -SNELHEID_INTEGRAAL_GRENS, SNELHEID_INTEGRAAL_GRENS);
  return commando;
}

void PidRegelaar::resetIntegraal() {
  integraal = 0.0f;
}
