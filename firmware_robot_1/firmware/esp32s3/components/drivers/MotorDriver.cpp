/**
 * @file MotorDriver.cpp
 * @brief Implementatie van MotorDriver met de LEDC-PWM van de ESP32-S3.
 */
#include "MotorDriver.hpp"

#include "Ontwerpparameters.hpp"

#include "driver/ledc.h"
#include "esp_log.h"

#include <algorithm>
#include <cstdlib>

namespace {
const char* TAG = "MotorDriver";

constexpr ledc_mode_t PWM_MODUS = LEDC_LOW_SPEED_MODE;
constexpr ledc_timer_t PWM_TIMER = LEDC_TIMER_0;
constexpr ledc_timer_bit_t PWM_RESOLUTIE = LEDC_TIMER_10_BIT;
constexpr uint32_t PWM_MAX_DUTY = (1 << 10) - 1;
constexpr uint32_t PWM_FREQUENTIE_HZ = 10000;  // gemeten: werkt met de ZK5AD

constexpr ledc_channel_t KANAAL_LINKS_A = LEDC_CHANNEL_0;
constexpr ledc_channel_t KANAAL_LINKS_B = LEDC_CHANNEL_1;
constexpr ledc_channel_t KANAAL_RECHTS_A = LEDC_CHANNEL_2;
constexpr ledc_channel_t KANAAL_RECHTS_B = LEDC_CHANNEL_3;

/// Koppelt een GPIO aan een PWM-kanaal met duty 0.
void configureerKanaal(ledc_channel_t kanaal, gpio_num_t pin) {
  ledc_channel_config_t config = {};
  config.gpio_num = pin;
  config.speed_mode = PWM_MODUS;
  config.channel = kanaal;
  config.intr_type = LEDC_INTR_DISABLE;
  config.timer_sel = PWM_TIMER;
  config.duty = 0;
  config.hpoint = 0;
  ESP_ERROR_CHECK(ledc_channel_config(&config));
}

/// Zet de duty van één kanaal.
bool zetDuty(ledc_channel_t kanaal, uint32_t duty) {
  return (ledc_set_duty(PWM_MODUS, kanaal, duty) == ESP_OK) &&
         (ledc_update_duty(PWM_MODUS, kanaal) == ESP_OK);
}

/// Stuurt één motor: PWM op A of B, afhankelijk van het teken.
/// vorige is de uitgang van de vorige aanroep (met teken, incl. startwaarde).
bool zetMotor(ledc_channel_t kanaalA, ledc_channel_t kanaalB, int16_t waarde, int32_t& vorige) {
  const int32_t begrensd = std::clamp<int32_t>(waarde, -MAX_MOTORUITGANG, MAX_MOTORUITGANG);
  // Dode-zonecompensatie: onder MOTOR_STARTWAARDE draait de motor niet, dus
  // elke uitgang die niet 0 is begint daar. 0 blijft 0 (stil).
  int32_t grootte = std::abs(begrensd);
  if (grootte > 0) {
    grootte = MOTOR_STARTWAARDE + grootte * (MAX_MOTORUITGANG - MOTOR_STARTWAARDE) / MAX_MOTORUITGANG;
  }
  const int32_t doel = (begrensd < 0) ? -grootte : grootte;

  // Stroompiekbegrenzing: per aanroep (5 ms) verandert de uitgang hoogstens
  // MOTOR_MAX_STAP. Een omkering van vol vooruit naar vol achteruit duurt zo
  // ca. 35 ms in plaats van 0 ms; de motor loopt daarbij even vrij uit. Die
  // harde omkeringen gaven stroompieken waardoor de S3 resette.
  // Stoppen (0) gebeurt altijd direct (FR-03, UC-S3-03, UC-S3-05).
  int32_t uitgang = 0;
  if (doel != 0) {
    uitgang = std::clamp<int32_t>(doel, vorige - MOTOR_MAX_STAP, vorige + MOTOR_MAX_STAP);
  }
  vorige = uitgang;

  const uint32_t duty = static_cast<uint32_t>(std::abs(uitgang)) * PWM_MAX_DUTY / MAX_MOTORUITGANG;
  if (uitgang >= 0) {
    return zetDuty(kanaalB, 0) && zetDuty(kanaalA, duty);
  }
  return zetDuty(kanaalA, 0) && zetDuty(kanaalB, duty);
}
}  // namespace

MotorDriver::MotorDriver(gpio_num_t linksA, gpio_num_t linksB,
                         gpio_num_t rechtsA, gpio_num_t rechtsB) {
  ledc_timer_config_t timerConfig = {};
  timerConfig.speed_mode = PWM_MODUS;
  timerConfig.duty_resolution = PWM_RESOLUTIE;
  timerConfig.timer_num = PWM_TIMER;
  timerConfig.freq_hz = PWM_FREQUENTIE_HZ;
  timerConfig.clk_cfg = LEDC_AUTO_CLK;
  ESP_ERROR_CHECK(ledc_timer_config(&timerConfig));

  configureerKanaal(KANAAL_LINKS_A, linksA);
  configureerKanaal(KANAAL_LINKS_B, linksB);
  configureerKanaal(KANAAL_RECHTS_A, rechtsA);
  configureerKanaal(KANAAL_RECHTS_B, rechtsB);
}

bool MotorDriver::stuur(int16_t links, int16_t rechts) {
  const bool ok = zetMotor(KANAAL_LINKS_A, KANAAL_LINKS_B, links, vorigeLinks) &&
                  zetMotor(KANAAL_RECHTS_A, KANAAL_RECHTS_B, rechts, vorigeRechts);
  if (!ok) {
    ESP_LOGE(TAG, "Motorcommando schrijven mislukt");
  }
  return ok;
}
