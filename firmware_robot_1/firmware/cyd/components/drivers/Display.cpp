/**
 * @file Display.cpp
 * @brief Implementatie van het STD van Display en het tekenen met drawables.
 */
#include "Display.hpp"

#include "Ontwerpparameters.hpp"
#include "TryReadQueue.hpp"

#include "esp_heap_caps.h"
#include "esp_log.h"

#include <algorithm>

namespace {
const char* TAG = "Display";

// Plaats van het gezicht
constexpr int OOG_Y = 80;
constexpr int OOG_LINKS_X = 105;
constexpr int OOG_RECHTS_X = 215;
constexpr int OOG_R = 32;
constexpr int MIDDEN_X = Ili9341Display::WIDTH / 2;
constexpr int MOND_Y = 148;

// Maten van een 7-segmentcijfer
constexpr int CIJFER_B = 26;
constexpr int CIJFER_H = 46;
constexpr int SEGMENT_D = 6;
constexpr int TUSSENRUIMTE = 12;

/// Segmenten per cijfer; bit 0..6 = segment a..g.
constexpr uint8_t SEGMENTEN[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

/// Tekent een lijn van 3 pixels dik met Line-drawables.
void tekenDikkeLijn(Ili9341Display& lcd, int x0, int y0, int x1, int y1, uint16_t kleur) {
  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      Line(x0 + dx, y0 + dy, x1 + dx, y1 + dy, kleur).draw(lcd);
    }
  }
}

/// Tekent een gevulde rechthoek; w en h zijn de echte breedte en hoogte.
void tekenBlok(Ili9341Display& lcd, int x, int y, int w, int h, uint16_t kleur) {
  // drawRectangle tekent tot en met x + w en y + h, vandaar de - 1.
  Rectangle(x, y, w - 1, h - 1, kleur, true).draw(lcd);
}
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

  xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes, this,
                          taskPriority, &taskHandle, taskCoreNumber);
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
  const int oogX = expressie.oogX;

  // Gezichtsdeel wissen; de score eronder blijft staan.
  tekenBlok(lcd, 0, 0, Ili9341Display::WIDTH, GEZICHT_HOOGTE, ACHTERGROND);

  switch (expressie.soort) {
    case ExpressieSoort::OPSTARTEN:
      // Slaperige ogen: alleen de onderste helft van het oog
      for (int x : ogen) {
        Circle(x, OOG_Y, OOG_R, wit, true).draw(lcd);
        tekenBlok(lcd, x - OOG_R - 1, OOG_Y - OOG_R - 1, 2 * OOG_R + 3, OOG_R + 1, ACHTERGROND);
        tekenDikkeLijn(lcd, x - OOG_R, OOG_Y, x + OOG_R, OOG_Y, wit);
      }
      tekenDikkeLijn(lcd, MIDDEN_X - 15, MOND_Y, MIDDEN_X + 15, MOND_Y, wit);
      break;

    case ExpressieSoort::BALANCEREN:
      // Ogen kijken mee met de kanteling
      for (int x : ogen) {
        Circle(x, OOG_Y, OOG_R, wit, true).draw(lcd);
        Circle(x + oogX, OOG_Y, 13, zwart, true).draw(lcd);
      }
      tekenDikkeLijn(lcd, MIDDEN_X - 30, MOND_Y, MIDDEN_X + 30, MOND_Y, wit);
      break;

    case ExpressieSoort::SCHRIKKEN:
      // Grote ogen, kleine pupillen, mond in een O
      for (int x : ogen) {
        Circle(x, OOG_Y, OOG_R + 6, wit, true).draw(lcd);
        Circle(x + oogX, OOG_Y, 7, zwart, true).draw(lcd);
      }
      for (int r = 13; r <= 16; r++) {
        Circle(MIDDEN_X, MOND_Y, r, wit).draw(lcd);
      }
      break;

    case ExpressieSoort::GEVALLEN:
      // Kruisjes als ogen en een boze mond
      for (int x : ogen) {
        tekenDikkeLijn(lcd, x - 20, OOG_Y - 20, x + 20, OOG_Y + 20, wit);
        tekenDikkeLijn(lcd, x - 20, OOG_Y + 20, x + 20, OOG_Y - 20, wit);
      }
      tekenDikkeLijn(lcd, MIDDEN_X - 30, MOND_Y + 10, MIDDEN_X, MOND_Y - 5, wit);
      tekenDikkeLijn(lcd, MIDDEN_X, MOND_Y - 5, MIDDEN_X + 30, MOND_Y + 10, wit);
      break;

    case ExpressieSoort::FOUT:
      // Rode blokogen en een zigzagmond
      for (int x : ogen) {
        tekenBlok(lcd, x - 25, OOG_Y - 15, 50, 30, rood);
      }
      for (int i = 0; i < 4; i++) {
        const int x0 = MIDDEN_X - 40 + i * 20;
        const int y0 = (i % 2 == 0) ? MOND_Y : MOND_Y - 10;
        const int y1 = (i % 2 == 0) ? MOND_Y - 10 : MOND_Y;
        tekenDikkeLijn(lcd, x0, y0, x0 + 20, y1, rood);
      }
      break;

    case ExpressieSoort::VERBINDING_VERLOREN:
      // Grijze, dichte ogen en drie puntjes: de gegevens zijn niet actueel
      for (int x : ogen) {
        tekenBlok(lcd, x - OOG_R, OOG_Y - 4, 2 * OOG_R, 8, grijs);
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
  tekenBlok(lcd, 0, GEZICHT_HOOGTE, Ili9341Display::WIDTH,
            Ili9341Display::HEIGHT - GEZICHT_HOOGTE, ACHTERGROND);
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
  const uint16_t geel = lcd.rgb565(255, 200, 0);
  const uint8_t segmenten = SEGMENTEN[cijfer % 10];
  const int B = CIJFER_B;
  const int H = CIJFER_H;
  const int D = SEGMENT_D;
  const int HALF = H / 2;

  if (segmenten & 0x01) tekenBlok(lcd, x + D, y, B - 2 * D, D, geel);                 // a
  if (segmenten & 0x02) tekenBlok(lcd, x + B - D, y + D, D, HALF - D, geel);          // b
  if (segmenten & 0x04) tekenBlok(lcd, x + B - D, y + HALF, D, HALF - D, geel);       // c
  if (segmenten & 0x08) tekenBlok(lcd, x + D, y + H - D, B - 2 * D, D, geel);         // d
  if (segmenten & 0x10) tekenBlok(lcd, x, y + HALF, D, HALF - D, geel);               // e
  if (segmenten & 0x20) tekenBlok(lcd, x, y + D, D, HALF - D, geel);                  // f
  if (segmenten & 0x40) tekenBlok(lcd, x + D, y + HALF - D / 2, B - 2 * D, D, geel);  // g
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
