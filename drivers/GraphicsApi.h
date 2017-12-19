#ifndef _GRAPHICS_API_H_
#define _GRAPHICS_API_H_

#include <cstddef>
#include <cstdint>

// TODO font
class GraphicsFont {
  virtual uint8_t getFontHeight() = 0;  // return the font height, in pixels

  virtual const uint8_t* getCharData(char in) = 0;  // returns a pointer to the character bit data
  virtual uint8_t getCharWidth(char in) = 0;  // returns the width of the character, in pixels
};

class GeneratorFont : GraphicsFont {
public:
  GeneratorFont(const uint8_t** charData, const uint8_t* charWidths, uint8_t height, uint8_t maxWidth) :
    charData_(charData), charWidths_(charWidths), height_(height), maxWidth_(maxWidth) {
  }

  uint8_t getFontHeight() {
    return height_;
  }

  const uint8_t* getCharData(char in) {
    if (in < 32 || in > 126) {
      return NULL;
    }
    return charData_[in - 32];
  }
  uint8_t getCharWidth(char in) {
    if (in < 32 || in > 126) {
      return 0;
    }
    return charWidths_[in - 32];
  }

private:
  const uint8_t** const charData_;
  const uint8_t* const charWidths_;
  const uint8_t height_;
  const uint8_t maxWidth_;
};

/**
 * Simple API for graphical displays, supporting color, monochrome, and one-bit displays.
 *
 * TODO: color display API
 *
 * Conventions:
 * Color is an optional argument on functions:
 * - if not specified: it defaults to the highest contrast color
 * - if specified as 8-bit int: used as the monochrome contrast, with 0 being lowest and 255 being highest
 * - if specified as a color: TODO
 * - colors are quantized as necessary, and colors are converted to monochrome by averaging the R, G, B components
 * Coordinates are defined with (0, 0) being the top left, increasing downwards and rightwards
 * - operations that exceed these bounds wrap
 */
class GraphicsApi {
public:
  /**
   * System-level commands
   */
  // Sends the framebuffer to the display
  virtual void update() = 0;

  /**
   * Parameters
   */
  // Returns the display width, in pixels
  virtual uint16_t getWidth() = 0;

  // Returns the display height, in pixels
  virtual uint16_t getHeight() = 0;

  /**
   * High level graphics operations
   */
  // Clears the framebuffer
  virtual void clear() {
    rectFilled(0, 0, getWidth() - 1, getHeight() - 1, 0);
  }

  // Draw a rectangle, coordinates are inclusive
  void rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    rect(x1, y1, x2, y2, 255);
  }
  virtual void rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t contrast) = 0;

  // Draw a filled rectangle, coordinates are inclusive
  void rectFilled(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    rectFilled(x1, y1, x2, y2, 255);
  }
  virtual void rectFilled(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t contrast) = 0;

  // Draw a line, using a fast line algorithm
  void line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    line(x1, y1, x2, y2, 255);
  }
  virtual void line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t contrast) = 0;

  // Draws text (null-terminated string), the specified location is the top left of the text drawn
  void text(uint16_t x, uint16_t y, const char* string, const GraphicsFont& font) {
    text(x, y, string, font, 255);
  }
  virtual void text(uint16_t x, uint16_t y, const char* string, const GraphicsFont& font, uint8_t contrast) = 0;

  // TODO: bitmapped image operations
};

#endif
