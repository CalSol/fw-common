#include "RgbActivityLed.h"

#include "LongTimer.h"

bool RgbActivity::update() {
  // if currently in a pulse, ensure the time is up before checking
  uint32_t currentUs = timebase_.read_us();
  if (!LongTimer::timePast(currentUs, currentPulseEndUs_)) {
    return false;
  }

  // If in idle, update timer to avoid overflow
  if (currentPulseColor_ == kCount) {
    currentPulseEndUs_ = currentUs;

    if (!needsUpdate_) {  // If don't need update, don't do anything more
      return false;
    }
  }

  RgbColor startingColor = currentPulseColor_;
  for (uint8_t colorIndex=(uint8_t)startingColor + 1; true; colorIndex++) {
    if (colorIndex > kCount) {
      colorIndex = kOff;
    }
    if (pulseRequestsMs_[colorIndex] > 0) {
      currentPulseColor_ = (RgbColor)colorIndex;
      currentPulseEndUs_ = currentPulseEndUs_ + pulseRequestsMs_[colorIndex] * 1000;

      if (colorIndex < kCount) {
        pulseRequestsMs_[colorIndex] = 0;  // clear the pulse request
        setRgb(currentPulseColor_);
      } else if (colorIndex == kCount) {
        if (startingColor == kCount) {
          needsUpdate_ = false;  // if we went through the entire array and found nothing, don't update
        }
        setRgb(idleColor_);
      }
      return true;
    }
  }
}

void RgbActivity::setIdle(RgbColor color) {
  idleColor_ = color;
  needsUpdate_ = true;
}

void RgbActivity::pulse(RgbColor color) {
  if (color >= kCount) {
    return;  // TODO this is an error - assert out? throw an exception?
  }
  if (color == idleColor_) {
    return;  // also ignored for obvious reasons
  }
  pulseRequestsMs_[color] = kPulseOnDurationMs;
  needsUpdate_ = true;
}

void RgbActivity::setRgb(RgbColor color) {
  switch (color) {
  case kRed:  setRgb(1, 0, 0);  break;
  case kGreen:  setRgb(0, 1, 0);  break;
  case kBlue:  setRgb(0, 0, 1);  break;
  case kYellow:  setRgb(1, 1, 0);  break;
  case kCyan:  setRgb(0, 1, 1);  break;
  case kPurple:  setRgb(1, 0, 1);  break;
  case kWhite:  setRgb(1, 1, 1);  break;
  default:
  case kOff:  setRgb(0, 0, 0);  break;
  }
}

void RgbActivityDigitalOut::setRgb(bool r, bool g, bool b) {
  if (onPolarity_) {
    r_ = r;
    g_ = g;
    b_ = b;
  } else {
    r_ = !r;
    g_ = !g;
    b_ = !b;
  }
}

void RgbActivityBitvector::setChannel(uint8_t* channelBitvector, uint8_t channelMask, bool on) {
  if (on) {
    *channelBitvector |= channelMask;
  } else {
    *channelBitvector &= ~channelMask;
  }
}

void RgbActivityBitvector::setRgb(bool r, bool g, bool b) {
  if (onPolarity_) {
    setChannel(r_, rMask_, r);
    setChannel(g_, gMask_, g);
    setChannel(b_, bMask_, b);
  } else {
    setChannel(r_, rMask_, !r);
    setChannel(g_, gMask_, !g);
    setChannel(b_, bMask_, !b);
  }
}
