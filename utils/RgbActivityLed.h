#ifndef _RGB_ACTIVITY_LED_H_
#define _RGB_ACTIVITY_LED_H_

#include "mbed.h"

const uint16_t kPulseOnDurationMs = 50;
const uint16_t kPulseRestDurationMs = 100;

/**
 * Abstract base class for RGB activity indicators.
 */
class RgbActivity {
public:
  enum RgbColor {
    kOff = 0,
    kRed,
    kGreen,
    kBlue,
    kYellow,
    kCyan,
    kPurple,
    kWhite,
    kCount  // also used as idle in currentPulseColor_
  };

  RgbActivity(Timer& timebase) :
    timebase_(timebase), idleColor_(kOff), currentPulseColor_(kCount),
    pulseRequestsMs_ {0}, needsUpdate_(true) {
      currentPulseEndUs_ = timebase_.read_us();
      pulseRequestsMs_[kCount] = kPulseRestDurationMs;
  }

  /**
   * Update the LED. Call this regularly.
   * Returns true if any of the LEDs may have changed.
   */
  bool update();

  /**
   * Sets the idle color (solid color when not pulsing)
   */
  void setIdle(RgbColor color);

  /**
   * Requests a pulse of the specified color of default duration.
   */
  void pulse(RgbColor color);

protected:
  /**
   * Set the RGB channels. Concrete subclasses must implement this.
   */
  virtual void setRgb(bool r, bool g, bool b) = 0;

  void setRgb(RgbColor color);

  Timer& timebase_;

  RgbColor idleColor_;  // color at resting state
  RgbColor currentPulseColor_;  // also used as index into pulseRequests
  uint32_t currentPulseEndUs_;  // time current pulse ends
  uint32_t pulseRequestsMs_[kCount + 1];  // pulse request, one element (ms pulse time) for each color

  bool needsUpdate_;
};

/**
 * RGB activity indicator using DigitalOut for channels.
 */
class RgbActivityDigitalOut : public RgbActivity {
public:
  RgbActivityDigitalOut(Timer& timebase,
      DigitalOut& r, DigitalOut& g, DigitalOut& b,
      bool onPolarity) :
      RgbActivity(timebase), r_(r), g_(g), b_(b), onPolarity_(onPolarity) {
  }

  void setRgb(bool r, bool g, bool b) ;

protected:
  DigitalOut& r_;
  DigitalOut& g_;
  DigitalOut& b_;

  const bool onPolarity_;  // value each channel is set to for "on"
};

/**
 * RGB activity indicator storing to a masked bit in a word.
 */
class RgbActivityBitvector : public RgbActivity {
public:
  RgbActivityBitvector(Timer& timebase,
      uint8_t* r, uint8_t rMask,
      uint8_t* g, uint8_t gMask,
      uint8_t* b, uint8_t bMask,
      bool onPolarity) :
      RgbActivity(timebase),
      r_(r), rMask_(rMask), g_(g), gMask_(gMask), b_(b), bMask_(bMask),
      onPolarity_(onPolarity) {
  }
  RgbActivityBitvector(Timer& timebase,
      uint8_t* bitVector, uint8_t rMask, uint8_t gMask, uint8_t bMask,
      bool onPolarity) :
      RgbActivity(timebase),
      r_(bitVector), rMask_(rMask), g_(bitVector), gMask_(gMask), b_(bitVector), bMask_(bMask),
      onPolarity_(onPolarity) {
  }


protected:
  void setChannel(uint8_t* channelBitvector, uint8_t channelMask, bool on);
  void setRgb(bool r, bool g, bool b);

  uint8_t* r_;
  uint8_t rMask_;
  uint8_t* g_;
  uint8_t gMask_;
  uint8_t* b_;
  uint8_t bMask_;

  const bool onPolarity_;  // value each channel is set to for "on"
};

#endif
