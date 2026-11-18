/**
 * @file main.cpp
 * @brief Meetprogramma voor de ESP32-S3: MPU6050, valtest en motorrichting.
 *
 * Dit is GEEN onderdeel van het ontwerp (geen STD's). Het gebruikt wel de
 * echte klassen Mpu6050, BalansFilter en MotorDriver, zodat de metingen
 * precies gelden voor de robotfirmware.
 *
 * Het programma loopt vanzelf door vijf stappen en zegt in de monitor wat je
 * moet doen. Kopieer aan het eind het blok tussen de streepjeslijnen.
 */
#include "BalansFilter.hpp"
#include "Meting.hpp"
#include "MetingListener.hpp"
#include "MotorDriver.hpp"
#include "Mpu6050.hpp"
#include "Ontwerpparameters.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {
// Zelfde pinnen als esp32s3/main/main.cpp
constexpr gpio_num_t PIN_SDA = GPIO_NUM_8;
constexpr gpio_num_t PIN_SCL = GPIO_NUM_9;
constexpr gpio_num_t PIN_LINKS_A = GPIO_NUM_5;
constexpr gpio_num_t PIN_LINKS_B = GPIO_NUM_4;
constexpr gpio_num_t PIN_RECHTS_A = GPIO_NUM_7;
constexpr gpio_num_t PIN_RECHTS_B = GPIO_NUM_6;

constexpr int METINGEN_PER_S = 1000 / REGELPERIODE;
constexpr float RAD_NAAR_GRADEN = 57.2957795f;

/// Zet elke meting van de Mpu6050 in een queue van lengte 1.
class TestListener : public MetingListener {
public:
  QueueHandle_t queue = xQueueCreate(1, sizeof(Meting));
  void metingGedaan(Meting meting) override { xQueueOverwrite(queue, &meting); }
};

/// Eén verwerkte meting.
struct Monster {
  Meting meting;
  float balans;   ///< uitkomst van BalansFilter (graden t.o.v. nulpunt)
  float accHoek;  ///< hoek alleen uit de accelerometer (graden)
};

TestListener* listener = nullptr;
BalansFilter* filter = nullptr;
int aantalLeesfouten = 0;

/// Wacht op de volgende meting en laat het filter meelopen.
Monster volgende() {
  Monster m;
  xQueueReceive(listener->queue, &m.meting, portMAX_DELAY);
  if (!m.meting.geldig) {
    aantalLeesfouten++;
  }
  m.balans = filter->bereken(m.meting);
  m.accHoek = std::atan2(-m.meting.ax, m.meting.az) * RAD_NAAR_GRADEN;
  return m;
}

/// Laat het filter een aantal seconden meelopen en telt hardop af.
void aftellen(int seconden) {
  for (int s = seconden; s > 0; s--) {
    printf("   ... %d\n", s);
    for (int i = 0; i < METINGEN_PER_S; i++) volgende();
  }
}

void kop(const char* titel, const char* opdracht) {
  printf("\n==================================================\n");
  printf("%s\n%s\n", titel, opdracht);
  printf("==================================================\n");
}
}  // namespace

extern "C" void app_main() {
  static MotorDriver motorDriver(PIN_LINKS_A, PIN_LINKS_B, PIN_RECHTS_A, PIN_RECHTS_B);
  static TestListener testListener;
  static BalansFilter balansFilter;
  static Mpu6050 mpu6050("Mpu6050", MPU6050_PRIORITEIT, MPU6050_STACK, CORE_REGELING,
                         PIN_SDA, PIN_SCL);
  listener = &testListener;
  filter = &balansFilter;
  motorDriver.stuur(0, 0);
  mpu6050.addListener(&testListener);
  mpu6050.start();

  // ---------------------------------------------------------------------
  kop("STAP 1 van 5: rechtop en stil (5 s)",
      "Motorvoeding UIT. Houd de robot rechtop in het punt waar hij bijna\n"
      "vanzelf blijft staan, en houd hem zo stil mogelijk.");
  aftellen(5);  // filter laten inlopen
  double somAx = 0, somAy = 0, somAz = 0, somGy = 0, somGy2 = 0;
  float maxGy = 0;
  const int n1 = 5 * METINGEN_PER_S;
  for (int i = 0; i < n1; i++) {
    const Monster m = volgende();
    somAx += m.meting.ax; somAy += m.meting.ay; somAz += m.meting.az;
    somGy += m.meting.gy; somGy2 += m.meting.gy * m.meting.gy;
    maxGy = std::max(maxGy, std::fabs(m.meting.gy));
  }
  const float gyOffset = somGy / n1;
  const float gyRuis = std::sqrt(std::max(0.0, somGy2 / n1 - gyOffset * gyOffset));
  const float ax0 = somAx / n1, ay0 = somAy / n1, az0 = somAz / n1;
  const float hoekRechtop = std::atan2(-ax0, az0) * RAD_NAAR_GRADEN;
  balansFilter.bepaalNulpunt();
  printf("Klaar. gy-offset %+.2f graden/s, ruis %.2f, max |gy| %.2f\n", gyOffset, gyRuis, maxGy);

  // ---------------------------------------------------------------------
  kop("STAP 2 van 5: kantelen (12 s)",
      "Motorvoeding UIT. Kantel EERST rustig ca. 20 graden VOOROVER (richting\n"
      "het scherm) en terug, daarna ca. 20 graden ACHTEROVER en terug.");
  aftellen(3);
  float maxBalans = 0, minBalans = 0, eersteUitslag = 0;
  double samenhang = 0;
  float vorigeAcc = NAN;
  const int n2 = 12 * METINGEN_PER_S;
  for (int i = 0; i < n2; i++) {
    const Monster m = volgende();
    maxBalans = std::max(maxBalans, m.balans);
    minBalans = std::min(minBalans, m.balans);
    if (eersteUitslag == 0 && std::fabs(m.balans) > 8.0f) eersteUitslag = m.balans;
    if (!std::isnan(vorigeAcc)) samenhang += m.meting.gy * (m.accHoek - vorigeAcc);
    vorigeAcc = m.accHoek;
    if (i % (METINGEN_PER_S / 5) == 0) {
      printf("t %4.1f s  balans %+6.1f  gy %+7.1f\n", (float)i / METINGEN_PER_S, m.balans,
             m.meting.gy);
    }
  }

  // ---------------------------------------------------------------------
  kop("STAP 3 van 5: valtest (3 keer)",
      "Motorvoeding UIT. Zet hem rechtop in het balanspunt. Laat hem LOS zodra\n"
      "'LOS' verschijnt en vang hem op voordat hij de grond raakt.");
  float omegaSom = 0;
  int omegaAantal = 0;
  for (int poging = 1; poging <= 3; poging++) {
    printf("\nPoging %d: zet hem rechtop...\n", poging);
    aftellen(4);
    printf("LOS!\n");
    float t10 = -1, t30 = -1, gyBij30 = 0, richting = 0;
    for (int i = 0; i < 3 * METINGEN_PER_S; i++) {
      const Monster m = volgende();
      const float t = (float)i / METINGEN_PER_S;
      const float a = std::fabs(m.balans);
      if (t10 < 0 && a >= 10.0f) { t10 = t; richting = m.balans; }
      if (t30 < 0 && a >= 30.0f) { t30 = t; gyBij30 = m.meting.gy; break; }
    }
    if (t10 >= 0 && t30 > t10) {
      const float omega = std::log(3.0f) / (t30 - t10);
      omegaSom += omega;
      omegaAantal++;
      printf("Gevallen %s: 10->30 graden in %.0f ms, omega %.1f /s, gy bij 30 graden %+.0f\n",
             richting > 0 ? "VOOROVER" : "ACHTEROVER", (t30 - t10) * 1000, omega, gyBij30);
    } else {
      printf("Geen val gemeten (niet ver genoeg of te langzaam). Volgende poging.\n");
    }
  }
  const float omega = omegaAantal > 0 ? omegaSom / omegaAantal : 0;

  // ---------------------------------------------------------------------
  kop("STAP 4 van 5: motorrichting",
      "Zet nu de MOTORVOEDING AAN. Houd de robot vast met de wielen VRIJ.\n"
      "Kijk of de wielen eerst VOORUIT (richting scherm) en dan ACHTERUIT gaan.");
  aftellen(8);
  printf(">>> VOORUIT (1 s)\n");
  motorDriver.stuur(500, 500);
  aftellen(1);
  motorDriver.stuur(0, 0);
  aftellen(2);
  printf(">>> ACHTERUIT (1 s)\n");
  motorDriver.stuur(-500, -500);
  aftellen(1);
  motorDriver.stuur(0, 0);

  // ---------------------------------------------------------------------
  kop("STAP 5 van 5: richting van de regeling (15 s)",
      "Wielen VRIJ. Kantel LICHT VOOROVER: de wielen moeten VOORUIT draaien.\n"
      "Kantel licht achterover: ACHTERUIT. (Alleen P, begrensd.)");
  aftellen(3);
  for (int i = 0; i < 15 * METINGEN_PER_S; i++) {
    const Monster m = volgende();
    int16_t u = static_cast<int16_t>(std::clamp(PID_KP * m.balans, -600.0f, 600.0f));
    if (std::fabs(m.balans) > 45.0f) u = 0;
    motorDriver.stuur(u, u);
    if (i % (METINGEN_PER_S / 4) == 0) {
      printf("balans %+6.1f  ->  motor %+5d (%s)\n", m.balans, u,
             u > 0 ? "vooruit" : (u < 0 ? "achteruit" : "stil"));
    }
  }
  motorDriver.stuur(0, 0);

  // ---------------------------------------------------------------------
  printf("\n----------------- KOPIEER VANAF HIER -----------------\n");
  printf("acc rechtop       ax %+.3f  ay %+.3f  az %+.3f g\n", ax0, ay0, az0);
  printf("hoek rechtop      %+.1f graden (alleen accelerometer)\n", hoekRechtop);
  printf("gy offset/ruis    %+.2f / %.2f graden/s (max %.2f)\n", gyOffset, gyRuis, maxGy);
  printf("kantelen          max %+.1f  min %+.1f  eerste uitslag %+.1f\n", maxBalans,
         minBalans, eersteUitslag);
  printf("gyro vs acc       %s (%.0f)\n", samenhang > 0 ? "zelfde richting" : "TEGENGESTELD",
         samenhang);
  printf("valtest           omega %.1f /s uit %d val(len), L = %.3f m\n", omega, omegaAantal,
         omega > 0 ? 9.81f / (omega * omega) : 0.0f);
  printf("leesfouten        %d\n", aantalLeesfouten);
  printf("----------------- TOT HIER -----------------\n");
  printf("Wat deden de wielen in stap 4 en stap 5? Zet dat er zelf bij.\n");
}
