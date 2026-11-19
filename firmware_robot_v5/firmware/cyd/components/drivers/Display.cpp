/**
 * @file Display.cpp
 * @brief Implementatie van het STD van Display en het tekenen met drawables.
 *
 * Tekenen gaat zoals in het lesmateriaal (TI-S3 Game_sprites): maak een
 * drawable (Circle, Rectangle of Line) en roep draw(lcd) aan. Alles komt in
 * de backbuffer en wordt pas met lcd.present() in één keer getoond.
 * Let op: drawRectangle(x, y, w, h) tekent tot en met x + w en y + h.
 */
#include "Display.hpp"

#include "Circle.hpp"
#include "Line.hpp"
#include "Ontwerpparameters.hpp"
#include "Rectangle.hpp"
#include "TryReadQueue.hpp"

#include "esp_heap_caps.h"
#include "esp_log.h"

#include <algorithm>

namespace {
const char* TAG = "Display";

// Gezicht: scherm 320 x 240, het gezicht staat in y = 0 .. GEZICHT_HOOGTE - 1.
constexpr int OOG_Y = 80;          ///< hoogte van het midden van de ogen
constexpr int OOG_LINKS_X = 105;   ///< midden van het linkeroog
constexpr int OOG_RECHTS_X = 215;  ///< midden van het rechteroog
constexpr int OOG_R = 32;          ///< straal van een oog
constexpr int PUPIL_R = 13;        ///< straal van een pupil
constexpr int MIDDEN_X = Ili9341Display::WIDTH / 2;
constexpr int MOND_Y = 145;        ///< hoogte van het midden van de mond
constexpr int MOND_R = 20;         ///< straal van de cirkels van lach en frons

// Score: drie 7-segmentcijfers onder het gezicht.
constexpr int CIJFER_B = 26;      ///< breedte van een cijfer
constexpr int CIJFER_H = 46;      ///< hoogte van een cijfer
constexpr int SEGMENT_D = 6;      ///< dikte van een segment
constexpr int TUSSENRUIMTE = 12;  ///< ruimte tussen twee cijfers

/// Segmenten per cijfer; bit 0..6 = segment a..g.
constexpr uint8_t SEGMENTEN[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
}  // namespace

Display::Display(const char* taskName, UBaseType_t taskPriority,
                 uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                 int mosi, int sclk, int cs, int dc, int rst, int bl)
    : lcd(mosi, sclk, cs, dc, rst, bl) {
  // De backbuffer bestaat uit twee helften van 76.800 bytes intern
  // DMA-geheugen (zie Ili9341Display.hpp). Die worden als eerste
  // gereserveerd, voor de WiFi-driver en de stacks van de andere taken.
  // Daarom staat lcd.init() hier en niet bij de pijl Uit -> Gereed.
  ESP_LOGI(TAG, "Grootste vrije DMA-blok voor lcd.init(): %u bytes",
           (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_DMA));
  lcd.init();

  eventGroup = xEventGroupCreate();
  configASSERT(eventGroup != NULL);
  expressieQueue = xQueueCreate(1, sizeof(Expressie));
  configASSERT(expressieQueue != NULL);
  scoreQueue = xQueueCreate(1, sizeof(uint32_t));
  configASSERT(scoreQueue != NULL);

  const BaseType_t gestart = xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes,
                                                     this, taskPriority, &taskHandle,
                                                     taskCoreNumber);
  configASSERT(gestart == pdPASS);
}

void Display::init() {
  xEventGroupSetBits(eventGroup, eventBitInit);
}

void Display::tekenGezicht(Expressie expressie) {
  xQueueOverwrite(expressieQueue, &expressie);
  xEventGroupSetBits(eventGroup, eventBitGezicht);
}

void Display::tekenScore(uint32_t score) {
  xQueueOverwrite(scoreQueue, &score);
  xEventGroupSetBits(eventGroup, eventBitScore);
}

void Display::tekenExpressie(Expressie expressie) {
  const uint16_t wit = lcd.rgb565(255, 255, 255);
  const uint16_t zwart = lcd.rgb565(0, 0, 0);
  const uint16_t rood = lcd.rgb565(255, 0, 0);
  const uint16_t grijs = lcd.rgb565(110, 110, 110);
  const int ogen[2] = {OOG_LINKS_X, OOG_RECHTS_X};

  // Gezichtsdeel wissen; de score eronder blijft staan.
  Rectangle(0, 0, Ili9341Display::WIDTH - 1, GEZICHT_HOOGTE - 1, ACHTERGROND, true).draw(lcd);

  switch (expressie.soort) {
    case ExpressieSoort::OPSTARTEN:
      // Halfopen ogen: een wit oog met een zwart blok over de bovenste helft
      for (int x : ogen) {
        Circle(x, OOG_Y, OOG_R, wit, true).draw(lcd);
        Rectangle(x - OOG_R, OOG_Y - OOG_R, 2 * OOG_R, OOG_R, ACHTERGROND, true).draw(lcd);
      }
      Rectangle(MIDDEN_X - 15, MOND_Y - 2, 30, 4, wit, true).draw(lcd);
      break;

    case ExpressieSoort::BALANCEREN:
      // De pupillen schuiven mee met de kanteling (oogX); de mond lacht.
      for (int x : ogen) {
        Circle(x, OOG_Y, OOG_R, wit, true).draw(lcd);
        Circle(x + expressie.oogX, OOG_Y, PUPIL_R, zwart, true).draw(lcd);
      }
      // Lach: een witte cirkel met een zwarte cirkel er iets boven
      Circle(MIDDEN_X, MOND_Y - 10, MOND_R, wit, true).draw(lcd);
      Circle(MIDDEN_X, MOND_Y - 16, MOND_R, ACHTERGROND, true).draw(lcd);
      break;

    case ExpressieSoort::SCHRIKKEN:
      // Grote ogen, kleine pupillen en een O-mond
      for (int x : ogen) {
        Circle(x, OOG_Y, OOG_R + 6, wit, true).draw(lcd);
        Circle(x + expressie.oogX, OOG_Y, 7, zwart, true).draw(lcd);
      }
      Circle(MIDDEN_X, MOND_Y, 16, wit, true).draw(lcd);
      Circle(MIDDEN_X, MOND_Y, 11, ACHTERGROND, true).draw(lcd);
      break;

    case ExpressieSoort::GEVALLEN:
      // Kruisjes als ogen: per streep 5 lijnen naast elkaar, zodat hij dik is
      for (int x : ogen) {
        for (int d = -2; d <= 2; d++) {
          Line(x - 20 + d, OOG_Y - 20, x + 20 + d, OOG_Y + 20, wit).draw(lcd);
          Line(x - 20 + d, OOG_Y + 20, x + 20 + d, OOG_Y - 20, wit).draw(lcd);
        }
      }
      // Frons: een witte cirkel met een zwarte cirkel er iets onder
      Circle(MIDDEN_X, MOND_Y + 8, MOND_R, wit, true).draw(lcd);
      Circle(MIDDEN_X, MOND_Y + 14, MOND_R, ACHTERGROND, true).draw(lcd);
      break;

    case ExpressieSoort::FOUT:
      // Rode, rechthoekige ogen en een rechte mond
      for (int x : ogen) {
        Rectangle(x - 25, OOG_Y - 15, 50, 30, rood, true).draw(lcd);
      }
      Rectangle(MIDDEN_X - 30, MOND_Y - 3, 60, 6, rood, true).draw(lcd);
      break;

    case ExpressieSoort::VERBINDING_VERLOREN:
      // Grijze, dichte ogen en drie puntjes: de gegevens zijn niet actueel
      for (int x : ogen) {
        Rectangle(x - OOG_R, OOG_Y - 4, 2 * OOG_R, 8, grijs, true).draw(lcd);
      }
      for (int i = -1; i <= 1; i++) {
        Circle(MIDDEN_X + i * 22, MOND_Y, 5, grijs, true).draw(lcd);
      }
      break;
  }
}

void Display::tekenScorebord(uint32_t score) {
  const uint16_t grijs = lcd.rgb565(110, 110, 110);

  // Scoredeel wissen; het gezicht erboven blijft staan.
  Rectangle(0, GEZICHT_HOOGTE, Ili9341Display::WIDTH - 1,
            Ili9341Display::HEIGHT - GEZICHT_HOOGTE - 1, ACHTERGROND, true).draw(lcd);
  Line(20, GEZICHT_HOOGTE + 2, Ili9341Display::WIDTH - 21, GEZICHT_HOOGTE + 2, grijs).draw(lcd);

  score = std::min(score, MAX_SCORE);
  const uint8_t cijfers[3] = {static_cast<uint8_t>(score / 100),
                              static_cast<uint8_t>((score / 10) % 10),
                              static_cast<uint8_t>(score % 10)};
  const int x0 = (Ili9341Display::WIDTH - (3 * CIJFER_B + 2 * TUSSENRUIMTE)) / 2;
  const int y = GEZICHT_HOOGTE + 8;
  bool tonen = false;  // voorloopnullen niet tekenen
  for (int i = 0; i < 3; i++) {
    tonen = tonen || (cijfers[i] != 0) || (i == 2);
    if (tonen) {
      tekenCijfer(cijfers[i], x0 + i * (CIJFER_B + TUSSENRUIMTE), y);
    }
  }
}

void Display::tekenCijfer(uint8_t cijfer, int x, int y) {
  //  aaa
  // f   b
  //  ggg
  // e   c
  //  ddd
  const uint16_t geel = lcd.rgb565(255, 200, 0);
  const uint8_t segmenten = SEGMENTEN[cijfer % 10];
  const int B = CIJFER_B;
  const int H = CIJFER_H;
  const int D = SEGMENT_D;
  const int HALF = H / 2;

  if (segmenten & 0x01) Rectangle(x + D, y, B - 2 * D, D, geel, true).draw(lcd);                 // a
  if (segmenten & 0x02) Rectangle(x + B - D, y + D, D, HALF - D, geel, true).draw(lcd);          // b
  if (segmenten & 0x04) Rectangle(x + B - D, y + HALF, D, HALF - D, geel, true).draw(lcd);       // c
  if (segmenten & 0x08) Rectangle(x + D, y + H - D, B - 2 * D, D, geel, true).draw(lcd);         // d
  if (segmenten & 0x10) Rectangle(x, y + HALF, D, HALF - D, geel, true).draw(lcd);               // e
  if (segmenten & 0x20) Rectangle(x, y + D, D, HALF - D, geel, true).draw(lcd);                  // f
  if (segmenten & 0x40) Rectangle(x + D, y + HALF - D / 2, B - 2 * D, D, geel, true).draw(lcd);  // g
}

void Display::taskWrapper(void* pParam) {
  static_cast<Display*>(pParam)->main();
}

void Display::main() {
  Expressie expressie;
  uint32_t score = 0;
  EventBits_t uxBits;

  while (true) {
    switch (state) {
      case State::Uit:
        xEventGroupWaitBits(eventGroup, eventBitInit, pdTRUE, pdFALSE, portMAX_DELAY);
        // lcd.init() is al in de constructor gedaan (zie daar).
        lcd.backlightOn();
        lcd.fillScreen(ACHTERGROND);
        lcd.present();
        state = State::Gereed;
        break;

      case State::Gereed:
        uxBits = xEventGroupWaitBits(eventGroup, eventBitGezicht | eventBitScore,
                                     pdFALSE, pdFALSE, portMAX_DELAY);
        // De score komt zelden; die eerst, zodat de vele gezichten hem niet
        // kunnen verdringen. Het andere bit blijft staan voor de volgende wait.
        if ((uxBits & eventBitScore) != 0) {
          if (tryReadQueue(scoreQueue, score, eventGroup, eventBitScore)) {
            tekenScorebord(score);
            lcd.present();
          }
        } else if ((uxBits & eventBitGezicht) != 0) {
          if (tryReadQueue(expressieQueue, expressie, eventGroup, eventBitGezicht)) {
            tekenExpressie(expressie);
            lcd.present();
          }
        }
        state = State::Gereed;
        break;
    }
  }
}
