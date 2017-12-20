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

  void rect(uint16_t x, uint16_t y, int16_t w, int16_t h, uint8_t contrast) {
    uint16_t x2 = x + w;
    uint16_t y2 = y + h;
    if (x2 < 0) {
      x2 = 0;
    }
    if (y2 < 0) {
      y2 = 0;
    }
    if (x2 < x) {
      std::swap(x, x2);
    }
    if (y2 < y) {
      std::swap(y, y2);
    }

    for (uint16_t xPos=x; xPos<x2; xPos++) {
      drawPixel(xPos, y, contrast);
      drawPixel(xPos, y2, contrast);
    }

    if ((y2 - y) > 1) {
      for (uint16_t yPos=y; yPos<y2; yPos++) {
        drawPixel(x, yPos, contrast);
        drawPixel(x2, yPos, contrast);
      }
    }
  }

  void rectFilled(uint16_t x, uint16_t y, int16_t w, int16_t h, uint8_t contrast) {
    uint16_t x2 = x + w;
    uint16_t y2 = y + h;
    if (x2 < 0) {
      x2 = 0;
    }
    if (y2 < 0) {
      y2 = 0;
    }
    if (x2 < x) {
      std::swap(x, x2);
    }
    if (y2 < y) {
      std::swap(y, y2);
    }

    for (uint16_t yPos=y; yPos<y2; yPos++) {
      for (uint16_t xPos=x; xPos<x2; xPos++) {
        drawPixel(xPos, yPos, contrast);
      }
    }
  }

  void line(uint16_t x, uint16_t y, int16_t w, int16_t h, uint8_t contrast) {
    int16_t dx = abs(w);
    int16_t dy = abs(h);

    int16_t xIncr = w < 0? -1 : 1;
    int16_t yIncr = h < 0? -1 : 1;

    if (dx >= dy) {
      // in x-major
      int16_t d = 2 * dy - dx;
      uint16_t yPos = y;
      uint16_t x2 = x + w;
      for (uint16_t xPos=x; xPos!=x2; xPos+=xIncr) {
        drawPixel(xPos, yPos, contrast);
        if (d > 0) {
          yPos += yIncr;
          d -= 2 * dx;
        }
        d += 2 * dy;
      }
    } else {
      // in y-major
      int16_t d = 2 * dx - dy;
      uint16_t xPos = x;
      uint16_t y2 = y + h;
      for (uint16_t yPos=y; yPos!=y2; yPos+=yIncr) {
        drawPixel(xPos, yPos, contrast);
        if (d > 0) {
          xPos += xIncr;
          d -= 2 * dy;
        }
        d += 2 * dx;
      }
    }
  }

  virtual uint16_t text(uint16_t x, uint16_t y, const char* string, GraphicsFont& font, uint8_t contrast) {
    uint16_t origx = x;
    for (; *string != 0; string++) {
      const uint8_t* charData = font.getCharData(*string) - 1;
      uint8_t charWidth = font.getCharWidth(*string);
      uint8_t charColBytes = charWidth / 8;
      if (charData != NULL) {
        for (uint8_t col=0; col<charWidth; col++) {
          if (x >= getWidth()) {
            return getWidth() - origx;
          }

          for (uint8_t row=0; row<font.getFontHeight(); row++) {
            uint8_t bitPos = row % 8;
            if (bitPos == 0) {
              charData++;
            }
            if (*charData & (1 << bitPos)) {
              if (y + row < getHeight()) {
                drawPixel(x, y + row, contrast);
              }
            }
          }

          x++;
        }
        x++;  // inter-character space
      }
    }
    return x - origx - 1;  // don't count the trailing space
  }

protected:
  uint8_t framebuffer_[256*64/2];  // in display memory order, byte=(MsNibble=1, LsNibble=0), x (row), y (col)
};

#endif
