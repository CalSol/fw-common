#ifndef _SSD1322_SPI_GRAPHICS_H_
#define _SSD1322_SPI_GRAPHICS_H_

#include <utility>

#include "Ssd1322Spi.h"
#include "GraphicsApi.h"

/**
 * SSD1322 over SPI using a framebuffer to expose a high level graphics API.
 *
 * TODO: don't make it a fixed 256*64 width
 */
class Ssd1322SpiGraphics: public Ssd1322Spi, public GraphicsApi {
public:
  Ssd1322SpiGraphics(SPI& spi, DigitalOut& cs, DigitalOut &dc, DigitalOut& reset) :
    Ssd1322Spi(spi, cs, dc, reset) {
  }

  void update() {
    beginWrite(0, 255, 0, 63);
    dc_ = 1;
    for (size_t i=0; i<sizeof(framebuffer_); i++) {
      spi_.write(framebuffer_[i]);
    }
    endWrite();
  }

  uint16_t getWidth() {
    return 256;
  }
  uint16_t getHeight() {
    return 64;
  }

  void drawPixel(uint16_t x, uint16_t y, uint8_t contrast) {
    bool writeMsNibble = (x % 2) == 0;
    contrast = contrast >> 4;
    framebuffer_[(y*128)+(x/2)] &= writeMsNibble ? 0x0f : 0xf0;  // unset pixel
    framebuffer_[(y*128)+(x/2)] |= contrast << (writeMsNibble ? 4 : 0);  // set pixel
  }

  // Allow API functions
  using GraphicsApi::rect;
  using GraphicsApi::rectFilled;
  using GraphicsApi::line;
  using GraphicsApi::text;

  void rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t contrast) {
    if (x2 < x1) {
      std::swap(x1, x2);
    }
    if (y2 < y1) {
      std::swap(y1, y2);
    }

    for (uint16_t x=x1; x<=x2; x++) {
      drawPixel(x, y1, contrast);
      drawPixel(x, y2, contrast);
    }

    if ((y2 - y1) > 1) {
      for (uint16_t y=y1; y<=y2; y++) {
        drawPixel(x1, y, contrast);
        drawPixel(x2, y, contrast);
      }
    }
  }

  void rectFilled(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t contrast) {
    for (uint16_t y=y1; y<=y2; y++) {
      for (uint16_t x=x1; x<=x2; x++) {
        drawPixel(x, y, contrast);
      }
    }
  }

  void line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t contrast) {
    int16_t dx = abs(x2 - x1);
    int16_t dy = abs(y2 - y1);

    int16_t d = 2 * dy - dx;
    int16_t xIncr = (x2 - x1) < 0? -1 : 1;
    int16_t yIncr = (y2 - y1) < 0? -1 : 1;

    if (dx >= dy) {
      // in x-major
      uint16_t y = y1;
      for (uint16_t x=x1; x!=x2; x+=xIncr) {
        drawPixel(x, y, contrast);
        if (d > 0) {
          y += yIncr;
          d -= 2 * dx;
        }
        d += 2 * dy;
      }
      drawPixel(x2, y, contrast);
    } else {
      // in y-major
      uint16_t x = x1;
      for (uint16_t y=y1; y!=y2; y+=yIncr) {
        drawPixel(x, y, contrast);
        if (d > 0) {
          x += xIncr;
          d -= 2 * dy;
        }
        d += 2 * dx;
      }
      drawPixel(x, y2, contrast);
    }
  }

  virtual void text(uint16_t x, uint16_t y, const char* string, const GraphicsFont& font, uint8_t contrast) {
    // TODO implement
  }

protected:
  uint8_t framebuffer_[256*64/2];  // in display memory order, byte=(MsNibble=1, LsNibble=0), x (row), y (col)
};

#endif
