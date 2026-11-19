/**
 * @file MotorDriver.cpp
 * @brief Implementatie van MotorDriver.
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
constexpr uint32_t PWM_FREQUENTIE_HZ = 10000;

constexpr ledc_channel_t KANAAL_LINKS_A = LEDC_CHANNEL_0;
constexpr ledc_channel_t KANAAL_LINKS_B = LEDC_CHANNEL_1;
constexpr ledc_channel_t KANAAL_RECHTS_A = LEDC_CHANNEL_2;
constexpr ledc_channel_t KANAAL_RECHTS_B = LEDC_CHANNEL_3;

// Elke motor heeft twee pinnen: A voor vooruit en B voor achteruit.
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

bool zetDuty(ledc_channel_t kanaal, uint32_t duty) {
  return (ledc_set_duty(PWM_MODUS, kanaal, duty) == ESP_OK) &&
         (ledc_update_duty(PWM_MODUS, kanaal) == ESP_OK);
}

bool zetMotor(ledc_channel_t kanaalA, ledc_channel_t kanaalB, int16_t waarde, int32_t& vorige) {
  const int32_t doel = std::clamp<int32_t>(waarde, -MAX_MOTORUITGANG, MAX_MOTORUITGANG);

  // Stoppen gaat direct, de rest geleidelijk.
  int32_t uitgang = 0;
  if (doel != 0) {
    uitgang = std::clamp<int32_t>(doel, vorige - MOTOR_MAX_STAP, vorige + MOTOR_MAX_STAP);
  }
  vorige = uitgang;

  const uint32_t duty = static_cast<uint32_t>(std::abs(uitgang)) * PWM_MAX_DUTY / MAX_MOTORUITGANG;
  const ledc_channel_t kanaalUit = (uitgang >= 0) ? kanaalB : kanaalA;
  const ledc_channel_t kanaalAan = (uitgang >= 0) ? kanaalA : kanaalB;
  const bool uitOk = zetDuty(kanaalUit, 0);
  const bool aanOk = zetDuty(kanaalAan, duty);
  return uitOk && aanOk;
}
}

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
  const bool linksOk = zetMotor(KANAAL_LINKS_A, KANAAL_LINKS_B, links, vorigeLinks);
  const bool rechtsOk = zetMotor(KANAAL_RECHTS_A, KANAAL_RECHTS_B, rechts, vorigeRechts);
  if (!linksOk || !rechtsOk) {
    ESP_LOGE(TAG, "Motorcommando schrijven mislukt");
  }
  return linksOk && rechtsOk;
}
