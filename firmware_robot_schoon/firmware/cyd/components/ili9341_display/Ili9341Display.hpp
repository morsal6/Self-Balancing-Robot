/*! \brief Headerfile for Ili9341 Display wrapper.
 *
 *   A series of projects to create a basic game-loop. Based on ./LCD/Drawables example code.
 *
 *  - Let the primitives make use of the backbuffer
 *  - Added a present() function to flush the backbuffer to the screen
 *  - Added Analog key input
 * 
 *  Use a backbuffer to reduce flickering, present the whole frame at once, and smooth motion (when combined with frame timer)
 * 
 * Implements key input that can move an entity.
 */
#pragma once

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_ili9341.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h" // for heap_caps_malloc
#include "esp_timer.h" // for esp_timer_get_time for fine grain timing

#include "Line.hpp"
#include "Rectangle.hpp"
#include "Circle.hpp"
#include "Triangle.hpp"
#include <vector>

// comment this out for the 2.4 inch display (or change it to DISPLAY24)
#define DISPLAY28

// Aangepast voor de robot: het scherm zit in de kop 180 graden gedraaid ten
// opzichte van de lesopstelling. Haal deze regel weg als het beeld dan juist
// ondersteboven staat.
#define DRAAI_180

class Ili9341Display {
public:
    Ili9341Display(int mosi, int sclk, int cs, int dc, int rst, int bl);
    ~Ili9341Display();

    // CYD in landscape: 320x240, different for CYD versions....
    static constexpr int WIDTH = 320;
    static constexpr int HEIGHT = 240;

    void init();
    void fillScreen(uint16_t color);
    void backlightOn();
    void backlightOff();
    void diagnostics();
    void diagnostics_drawables();
    uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b);

    // Drawing Primitives
    void drawPixel(int x, int y, uint16_t color);
    void drawLine(int x0, int y0, int x1, int y1, uint16_t color);
    void drawRectangle(int x, int y, int w, int h, uint16_t color, bool fill = false);
    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, bool fill = false);
    void drawCircle(int x, int y, int radius, uint16_t color, bool fill = false);
    // Drawing bitmaps
    void drawBitmapRGB565(int x, int y, int w, int h, const uint8_t* src);
    void drawBitmapRGB565(int x, int y, int w, int h, const uint16_t* src16);

    void present(); // push the backbuffer to the LCD panel
private:
    static constexpr const char* TAG = "Game_sprites";

    void setupSPI();
    void setupPanel();

    int _mosi, _sclk, _cs, _dc, _rst, _bl;
    spi_host_device_t _host = SPI2_HOST;
    esp_lcd_panel_io_handle_t _io = nullptr;
    esp_lcd_panel_handle_t _panel = nullptr;

    // For the backbuffer.
    // Aangepast voor de robot: de backbuffer bestaat uit twee helften
    // (boven en onder) van elk WIDTH * HALF_HEIGHT pixels. Met WiFi aan is er
    // op de ESP32 geen aaneengesloten blok van 153.600 bytes vrij (gemeten:
    // 139.264 bytes), wel twee blokken van 76.800 bytes.
    static constexpr int HALF_HEIGHT = HEIGHT / 2;
    uint16_t* backbuffer_[2] = {nullptr, nullptr};  // [0] = boven, [1] = onder
    size_t    backbuffer_len_ = 0;       // number of uint16_t entries per half

    inline bool hasBuffer() const { return backbuffer_[0] != nullptr && backbuffer_[1] != nullptr; }
};