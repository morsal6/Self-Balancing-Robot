/**
 * @file main.cpp
 * @brief Meetprogramma voor de S3: sensor, motoren en een balanceertest met logboek.
 */
#include "BalansFilter.hpp"
#include "Meting.hpp"
#include "MetingListener.hpp"
#include "MotorDriver.hpp"
#include "Mpu6050.hpp"
#include "Ontwerpparameters.hpp"
#include "PidRegelaar.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {
constexpr bool ALLEEN_BALANSTEST = true;  // true: alleen stap 1 en stap 6

constexpr gpio_num_t PIN_SDA = GPIO_NUM_8;
constexpr gpio_num_t PIN_SCL = GPIO_NUM_9;
constexpr gpio_num_t PIN_LINKS_A = GPIO_NUM_5;
constexpr gpio_num_t PIN_LINKS_B = GPIO_NUM_4;
constexpr gpio_num_t PIN_RECHTS_A = GPIO_NUM_7;
constexpr gpio_num_t PIN_RECHTS_B = GPIO_NUM_6;

constexpr int METINGEN_PER_S = 1000 / REGELPERIODE;
constexpr float RAD_NAAR_GRADEN = 57.2957795f;

class TestListener : public MetingListener {
public:
  QueueHandle_t queue = xQueueCreate(1, sizeof(Meting));
  void metingGedaan(Meting meting) override { xQueueOverwrite(queue, &meting); }
};

struct Monster {
  Meting meting;
  float balans;
  float accHoek;
};

struct LogRegel {
  uint16_t tMs;
  int16_t balans10;
  int16_t gy10;
  int16_t u;
};

struct Poging {
  const char* uitkomst;
  float tijd;
  float gemU;
  float verzadigd;
  float wisselsPerS;
  float maxBalans;
  int aantalRegels;
};

constexpr int BALANS_POGINGEN = 3;
constexpr int BALANS_MAX_S = 60;  // NFR-04: 60 s blijven staan
constexpr int LOG_STAP = 20;      // elke 100 ms een regel
constexpr int LOG_REGELS = BALANS_MAX_S * METINGEN_PER_S / LOG_STAP;
LogRegel logboek[BALANS_POGINGEN][LOG_REGELS];
Poging pogingen[BALANS_POGINGEN];

TestListener* listener = nullptr;
BalansFilter* filter = nullptr;
int aantalLeesfouten = 0;

// Wacht op de volgende meting en laat het filter meelopen.
Monster volgende() {
  static Monster vorige = {};
  Monster m;
  xQueueReceive(listener->queue, &m.meting, portMAX_DELAY);
  if (!m.meting.geldig) {
    aantalLeesfouten++;
    Monster fout = vorige;
    fout.meting.geldig = false;  // balanceer() stopt hierop, net als de robot (Fout)
    return fout;
  }
  m.balans = filter->bereken(m.meting);
  m.accHoek = std::atan2(-m.meting.ax, m.meting.az) * RAD_NAAR_GRADEN;
  vorige = m;
  return m;
}

void aftellen(int seconden) {
  for (int s = seconden; s > 0; s--) {
    printf("   ... %d\n", s);
    for (int i = 0; i < METINGEN_PER_S; i++) volgende();
  }
}

void draai(MotorDriver& motorDriver, int16_t uitgang, int seconden) {
  for (int i = 0; i < seconden * METINGEN_PER_S; i++) {
    volgende();
    motorDriver.stuur(uitgang, uitgang);
  }
  motorDriver.stuur(0, 0);
}

// Doet hetzelfde als BalansControl::verwerkMeting. Het logboek gaat eerst naar
// het geheugen en wordt pas na afloop geprint, anders klopt de timing niet.
void balanceer(MotorDriver& motorDriver, PidRegelaar& pid, int nr) {
  printf("\nPoging %d van %d: zet hem rechtop op de vloer en houd hem licht vast...\n", nr + 1,
         BALANS_POGINGEN);
  aftellen(5);
  printf("LOS!\n");
  pid.init();
  Poging& p = pogingen[nr];
  p = {"bleef staan", (float)BALANS_MAX_S, 0, 0, 0, 0, 0};
  int buitenGrens = 0, geregeld = 0, verzadigd = 0, wissels = 0;
  int16_t vorigeU = 0;
  double somU = 0;
  int i = 0;
  for (; i < BALANS_MAX_S * METINGEN_PER_S; i++) {
    const Monster m = volgende();
    int16_t u = vorigeU;
    if (!m.meting.geldig) {
      p.uitkomst = "SENSORFOUT";
      break;
    }
    if (std::fabs(m.balans) > BALANSVERLIESGRENS) {
      if (++buitenGrens >= BEVESTIGINGSAANTAL) {
        p.uitkomst = (m.balans > 0) ? "VOOROVER gevallen" : "ACHTEROVER gevallen";
        break;
      }
    } else {
      buitenGrens = 0;
      u = pid.bereken(m.balans);
      motorDriver.stuur(u, u);
      geregeld++;
      somU += u;
      if (std::abs(u) >= MAX_MOTORUITGANG) verzadigd++;
      if ((u > 0 && vorigeU < 0) || (u < 0 && vorigeU > 0)) wissels++;
      p.maxBalans = std::max(p.maxBalans, std::fabs(m.balans));
    }
    vorigeU = u;
    if (i % LOG_STAP == 0 && p.aantalRegels < LOG_REGELS) {
      logboek[nr][p.aantalRegels++] = {static_cast<uint16_t>(i * REGELPERIODE),
                                       static_cast<int16_t>(m.balans * 10),
                                       static_cast<int16_t>(m.meting.gy * 10), u};
    }
  }
  motorDriver.stuur(0, 0);
  p.tijd = (float)i / METINGEN_PER_S;
  if (geregeld > 0) {
    p.gemU = somU / geregeld;
    p.verzadigd = (float)verzadigd / geregeld;
  }
  if (p.tijd > 0) p.wisselsPerS = wissels / p.tijd;
  printf("%s na %.2f s\n", p.uitkomst, p.tijd);
}

void kop(const char* titel, const char* opdracht) {
  printf("\n==================================================\n");
  printf("%s\n%s\n", titel, opdracht);
  printf("==================================================\n");
}
}

extern "C" void app_main() {
  static MotorDriver motorDriver(PIN_LINKS_A, PIN_LINKS_B, PIN_RECHTS_A, PIN_RECHTS_B);
  static TestListener testListener;
  static BalansFilter balansFilter;
  static Mpu6050 mpu6050("Mpu6050", MPU6050_PRIORITEIT, MPU6050_STACK, CORE_REGELING,
                         PIN_SDA, PIN_SCL);
  static PidRegelaar pid(PID_KP, PID_KI, PID_KD);
  listener = &testListener;
  filter = &balansFilter;
  motorDriver.stuur(0, 0);
  mpu6050.addListener(&testListener);
  mpu6050.start();

  kop("STAP 1: rechtop en stil (5 s)",
      "Motorvoeding UIT. Houd de robot rechtop in het punt waar hij bijna\n"
      "vanzelf blijft staan, en houd hem zo stil mogelijk.");
  aftellen(5);
  double somAx = 0, somAy = 0, somAz = 0, somGy = 0, somGy2 = 0;
  float maxGy = 0;
  const int n1 = 5 * METINGEN_PER_S;
  for (int i = 0; i < n1; i++) {
    const Monster m = volgende();
    somAx += m.meting.ax; somAy += m.meting.ay; somAz += m.meting.az;
    somGy += m.meting.gy; somGy2 += m.meting.gy * m.meting.gy;
    maxGy = std::max(maxGy, std::fabs(m.meting.gy));
  }

  const float gyRest = somGy / n1;
  const float gyRuis = std::sqrt(std::max(0.0, somGy2 / n1 - gyRest * gyRest));
  const float ax0 = somAx / n1, ay0 = somAy / n1, az0 = somAz / n1;
  const float hoekRechtop = std::atan2(-ax0, az0) * RAD_NAAR_GRADEN;
  balansFilter.bepaalNulpunt();
  printf("Klaar. Resterende gy-offset %+.2f graden/s (GY_OFFSET wordt %+.2f), ruis %.2f\n",
         gyRest, GY_OFFSET + gyRest, gyRuis);

  float maxBalans = 0, minBalans = 0, eersteUitslag = 0;
  double samenhang = 0;
  float omega = 0;
  int omegaAantal = 0;
  if (!ALLEEN_BALANSTEST) {
    kop("STAP 2: kantelen (12 s)",
        "Motorvoeding UIT. Kantel EERST rustig ca. 20 graden VOOROVER (richting\n"
        "het scherm) en terug, daarna ca. 20 graden ACHTEROVER en terug.");
    aftellen(3);
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

    kop("STAP 3: valtest (3 keer)",
        "Motorvoeding UIT. Zet hem rechtop in het balanspunt. Laat hem LOS zodra\n"
        "'LOS' verschijnt en vang hem op voordat hij de grond raakt.");
    float omegaSom = 0;
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
        const float omegaPoging = std::log(3.0f) / (t30 - t10);
        omegaSom += omegaPoging;
        omegaAantal++;
        printf("Gevallen %s: 10->30 graden in %.0f ms, omega %.1f /s, gy bij 30 graden %+.0f\n",
               richting > 0 ? "VOOROVER" : "ACHTEROVER", (t30 - t10) * 1000, omegaPoging, gyBij30);
      } else {
        printf("Geen val gemeten (niet ver genoeg of te langzaam). Volgende poging.\n");
      }
    }
    omega = omegaAantal > 0 ? omegaSom / omegaAantal : 0;

    kop("STAP 4: motorrichting",
        "Zet nu de MOTORVOEDING AAN. Houd de robot vast met de wielen VRIJ.\n"
        "Kijk of de wielen eerst VOORUIT (richting scherm) en dan ACHTERUIT gaan.");
    aftellen(8);
    printf(">>> VOORUIT (1 s)\n");
    draai(motorDriver, 800, 1);
    aftellen(2);
    printf(">>> ACHTERUIT (1 s)\n");
    draai(motorDriver, -800, 1);

    kop("STAP 5: richting van de regeling (15 s)",
        "Wielen VRIJ. Kantel LICHT VOOROVER: de wielen moeten VOORUIT draaien.\n"
        "Kantel licht achterover: ACHTERUIT. (Vanaf 3 graden, vaste uitgang.)");
    aftellen(3);
    for (int i = 0; i < 15 * METINGEN_PER_S; i++) {
      const Monster m = volgende();

      int16_t u = 0;
      if (m.balans > 3.0f) u = 800;
      if (m.balans < -3.0f) u = -800;
      if (std::fabs(m.balans) > 45.0f) u = 0;
      motorDriver.stuur(u, u);
      if (i % (METINGEN_PER_S / 4) == 0) {
        printf("balans %+6.1f  ->  motor %+5d (%s)\n", m.balans, u,
               u > 0 ? "vooruit" : (u < 0 ? "achteruit" : "stil"));
      }
    }
    motorDriver.stuur(0, 0);
  }

  kop("STAP 6: balanceertest (3 pogingen van max. 60 s)",
      "Zet nu de MOTORVOEDING AAN. Zet de robot op een vlakke vloer met ruimte\n"
      "voor en achter. Houd hem licht vast in het balanspunt en laat LOS zodra\n"
      "'LOS' verschijnt. Rijdt hij weg of valt hij: vang hem op.");
  aftellen(5);
  for (int nr = 0; nr < BALANS_POGINGEN; nr++) {
    balanceer(motorDriver, pid, nr);
  }

  printf("\n----------------- KOPIEER VANAF HIER -----------------\n");
  printf("PID KP %.1f KI %.1f KD %.1f  FILTER_ALFA %.3f  STARTWAARDE %d\n", PID_KP, PID_KI,
         PID_KD, FILTER_ALFA, MOTOR_STARTWAARDE);
  printf("SNELHEID KP %.1f KI %.1f FILTERTIJD %.2f s GRENS %.1f\n", SNELHEID_KP, SNELHEID_KI,
         SNELHEID_FILTERTIJD, SNELHEID_INTEGRAAL_GRENS);
  printf("acc rechtop       ax %+.3f  ay %+.3f  az %+.3f g\n", ax0, ay0, az0);
  printf("hoek rechtop      %+.1f graden (alleen accelerometer)\n", hoekRechtop);
  printf("gy rest-offset    %+.2f graden/s  ->  GY_OFFSET %+.2f\n", gyRest, GY_OFFSET + gyRest);
  printf("gy ruis           %.2f graden/s (max |gy| %.2f)\n", gyRuis, maxGy);
  if (!ALLEEN_BALANSTEST) {
    printf("kantelen          max %+.1f  min %+.1f  eerste uitslag %+.1f\n", maxBalans,
           minBalans, eersteUitslag);
    printf("gyro vs acc       %s (%.0f)\n", samenhang > 0 ? "zelfde richting" : "TEGENGESTELD",
           samenhang);
    printf("valtest           omega %.1f /s uit %d val(len), L = %.3f m\n", omega, omegaAantal,
           omega > 0 ? 9.81f / (omega * omega) : 0.0f);
  }
  printf("leesfouten        %d\n", aantalLeesfouten);
  for (int nr = 0; nr < BALANS_POGINGEN; nr++) {
    const Poging& p = pogingen[nr];
    printf("poging %d          %s na %.2f s, gem. u %+.0f, vol %.0f%%, wissels %.1f/s, max %.1f gr\n",
           nr + 1, p.uitkomst, p.tijd, p.gemU, p.verzadigd * 100, p.wisselsPerS, p.maxBalans);
  }
  for (int nr = 0; nr < BALANS_POGINGEN; nr++) {
    printf("# poging %d: t(s) balans gy u\n", nr + 1);
    for (int r = 0; r < pogingen[nr].aantalRegels; r++) {
      const LogRegel& l = logboek[nr][r];
      printf("%.2f %+.1f %+.1f %+d\n", l.tMs / 1000.0f, l.balans10 / 10.0f, l.gy10 / 10.0f, l.u);
    }
  }
  printf("----------------- TOT HIER -----------------\n");
  if (!ALLEEN_BALANSTEST) {
    printf("Wat deden de wielen in stap 4 en stap 5? Zet dat er zelf bij.\n");
  }
}
