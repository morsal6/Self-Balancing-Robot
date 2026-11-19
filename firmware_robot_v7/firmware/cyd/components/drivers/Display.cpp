/**
 * @file Display.cpp
 * @brief Implementatie van Display.
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

// Scherm is 320 x 240, het gezicht staat boven GEZICHT_HOOGTE.
constexpr int OOG_Y = 80;
constexpr int OOG_LINKS_X = 105;
constexpr int OOG_RECHTS_X = 215;
constexpr int OOG_R = 32;
constexpr int PUPIL_R = 13;
constexpr int MIDDEN_X = Ili9341Display::WIDTH / 2;
constexpr int MOND_Y = 145;
constexpr int MOND_R = 20;

// Score met drie 7-segment cijfers
constexpr int CIJFER_B = 26;
constexpr int CIJFER_H = 46;
constexpr int SEGMENT_D = 6;
constexpr int TUSSENRUIMTE = 12;

constexpr uint8_t SEGMENTEN[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};  // bit 0..6 = segment a..g
}

Display::Display(const char* taskName, UBaseType_t taskPriority,
                 uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                 int mosi, int sclk, int cs, int dc, int rst, int bl)
    : lcd(mosi, sclk, cs, dc, rst, bl) {
  ESP_LOGI(TAG, "Grootste vrije DMA-blok voor lcd.init(): %u bytes",
           (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_DMA));
  // lcd.init() hier en niet in Uit -> Gereed: de backbuffer heeft veel DMA-geheugen
  // nodig en dat moet eerst gereserveerd worden, voor WiFi het pakt.
  lcd.init();
  lcd.backlightOff();  // lcd.init() zet hem aan, maar dat hoort bij Uit -> Gereed

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

  // Alleen het gezicht wissen. Let op: drawRectangle tekent tot en met x + w en y + h.
  Rectangle(0, 0, Ili9341Display::WIDTH - 1, GEZICHT_HOOGTE - 1, ACHTERGROND, true).draw(lcd);

  switch (expressie.soort) {
    case ExpressieSoort::OPSTARTEN:
      for (int x : ogen) {
        // halfopen ogen
        Circle(x, OOG_Y, OOG_R, wit, true).draw(lcd);
        Rectangle(x - OOG_R, OOG_Y - OOG_R, 2 * OOG_R, OOG_R, ACHTERGROND, true).draw(lcd);
      }
      Rectangle(MIDDEN_X - 15, MOND_Y - 2, 30, 4, wit, true).draw(lcd);
      break;

    case ExpressieSoort::BALANCEREN:
      for (int x : ogen) {
        // pupillen kijken mee (oogX)
        Circle(x, OOG_Y, OOG_R, wit, true).draw(lcd);
        Circle(x + expressie.oogX, OOG_Y, PUPIL_R, zwart, true).draw(lcd);
      }

      // lach: witte cirkel met een zwarte cirkel iets erboven
      Circle(MIDDEN_X, MOND_Y - 10, MOND_R, wit, true).draw(lcd);
      Circle(MIDDEN_X, MOND_Y - 16, MOND_R, ACHTERGROND, true).draw(lcd);
      break;

    case ExpressieSoort::SCHRIKKEN:
      for (int x : ogen) {
        // grote ogen, kleine pupillen en een O-mond
        Circle(x, OOG_Y, OOG_R + 6, wit, true).draw(lcd);
        Circle(x + expressie.oogX, OOG_Y, 7, zwart, true).draw(lcd);
      }
      Circle(MIDDEN_X, MOND_Y, 16, wit, true).draw(lcd);
      Circle(MIDDEN_X, MOND_Y, 11, ACHTERGROND, true).draw(lcd);
      break;

    case ExpressieSoort::GEVALLEN:
      for (int x : ogen) {
        // kruisjes, 5 lijnen naast elkaar voor een dikke lijn
        for (int d = -2; d <= 2; d++) {
          Line(x - 20 + d, OOG_Y - 20, x + 20 + d, OOG_Y + 20, wit).draw(lcd);
          Line(x - 20 + d, OOG_Y + 20, x + 20 + d, OOG_Y - 20, wit).draw(lcd);
        }
      }

      // frons: zwarte cirkel iets eronder
      Circle(MIDDEN_X, MOND_Y + 8, MOND_R, wit, true).draw(lcd);
      Circle(MIDDEN_X, MOND_Y + 14, MOND_R, ACHTERGROND, true).draw(lcd);
      break;

    case ExpressieSoort::FOUT:
      for (int x : ogen) {
        // rode ogen
        Rectangle(x - 25, OOG_Y - 15, 50, 30, rood, true).draw(lcd);
      }
      Rectangle(MIDDEN_X - 30, MOND_Y - 3, 60, 6, rood, true).draw(lcd);
      break;

    case ExpressieSoort::VERBINDING_VERLOREN:
      for (int x : ogen) {
        // dichte grijze ogen en ...
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

  // Alleen de score wissen.
  Rectangle(0, GEZICHT_HOOGTE, Ili9341Display::WIDTH - 1,
            Ili9341Display::HEIGHT - GEZICHT_HOOGTE - 1, ACHTERGROND, true).draw(lcd);
  Line(20, GEZICHT_HOOGTE + 2, Ili9341Display::WIDTH - 21, GEZICHT_HOOGTE + 2, grijs).draw(lcd);

  score = std::min(score, MAX_SCORE);
  const uint8_t cijfers[3] = {static_cast<uint8_t>(score / 100),
                              static_cast<uint8_t>((score / 10) % 10),
                              static_cast<uint8_t>(score % 10)};
  const int x0 = (Ili9341Display::WIDTH - (3 * CIJFER_B + 2 * TUSSENRUIMTE)) / 2;
  const int y = GEZICHT_HOOGTE + 8;
  bool tonen = false;  // geen voorloopnullen
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

  if (segmenten & 0x01) Rectangle(x + D, y, B - 2 * D, D, geel, true).draw(lcd);
  if (segmenten & 0x02) Rectangle(x + B - D, y + D, D, HALF - D, geel, true).draw(lcd);
  if (segmenten & 0x04) Rectangle(x + B - D, y + HALF, D, HALF - D, geel, true).draw(lcd);
  if (segmenten & 0x08) Rectangle(x + D, y + H - D, B - 2 * D, D, geel, true).draw(lcd);
  if (segmenten & 0x10) Rectangle(x, y + HALF, D, HALF - D, geel, true).draw(lcd);
  if (segmenten & 0x20) Rectangle(x, y + D, D, HALF - D, geel, true).draw(lcd);
  if (segmenten & 0x40) Rectangle(x + D, y + HALF - D / 2, B - 2 * D, D, geel, true).draw(lcd);
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

        lcd.backlightOn();
        lcd.fillScreen(ACHTERGROND);
        lcd.present();
        state = State::Gereed;
        break;

      case State::Gereed:
        uxBits = xEventGroupWaitBits(eventGroup, eventBitGezicht | eventBitScore,
                                     pdFALSE, pdFALSE, portMAX_DELAY);

        // Score eerst, die komt zelden. Het andere bit blijft staan voor de volgende wait.
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
