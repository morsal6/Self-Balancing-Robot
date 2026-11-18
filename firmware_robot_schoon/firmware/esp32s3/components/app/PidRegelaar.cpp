/**
 * @file PidRegelaar.cpp
 * @brief Implementatie van PidRegelaar.
 */
#include "PidRegelaar.hpp"

#include "Ontwerpparameters.hpp"

#include <algorithm>

PidRegelaar::PidRegelaar(float kp, float ki, float kd) : kp(kp), ki(ki), kd(kd) {}

void PidRegelaar::init() {
  integraal = 0.0f;
  vorigeAfwijking = 0.0f;
}

int16_t PidRegelaar::bereken(float afwijking) {
  // Anti-windup: de integraal wordt begrensd.
  integraal = std::clamp(integraal + afwijking * REGELPERIODE_S, -INTEGRAAL_GRENS, INTEGRAAL_GRENS);
  const float differentiaal = (afwijking - vorigeAfwijking) / REGELPERIODE_S;
  vorigeAfwijking = afwijking;

  const float uitgang = kp * afwijking + ki * integraal + kd * differentiaal;
  const float begrensd = std::clamp(uitgang, static_cast<float>(-MAX_MOTORUITGANG),
                                    static_cast<float>(MAX_MOTORUITGANG));
  return static_cast<int16_t>(begrensd);
}

void PidRegelaar::resetIntegraal() {
  integraal = 0.0f;
}
