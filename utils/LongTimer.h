#ifndef _LONG_TIMER_H
#define _LONG_TIMER_H

#include <atomic>

#include "mbed.h"

/**
 * Long timer that is effectively a 64-bit us timer.
 *
 * Does NOT start started.
 *
 * Uses the mbed timer API with an extra 32-bit overflow timer.
 * update() must only be called from one thread, but read_us and read_ms are
 * interrupt safe and do not modify internal state.
 * read_us and read_ms are NOT completely thread-safe, but should be fine in
 * almost all cases (where threads are switched much faster than the us timer
 * overflow period).
 */
class LongTimer {
public:
  LongTimer(Timer& usTimer) : usTimer_(usTimer) {
    currentIndex_.store(0);
    for (uint8_t i=0; i<2; i++) {
      lastUs_[i].store(0);
      usRollovers_[i].store(0);
    }
  }

  /**
   * Call this every once in a while (no longer than every 30 minutes) to check
   * for overflows and update internal state as necessary.
   */
  void update();

  /**
   * Returns the 32-bit time in microseconds. Fast.
   */
  uint32_t read_short_us();

  /**
   * Returns the current 64-bit time in microseconds.
   * Does not modify internal state, safe to call inside an interrupt.
   */
  uint64_t read_us();

  /**
   * Returns the current 32-bit time in milliseconds.
   * Does not modify internal state, safe to call inside an interrupt.
   */
  uint32_t read_ms();

  /**
   * Compare two times, allowing for overflow. Helper method.
   * TODO: perhaps move elsewhere?
   */
  static inline uint32_t timePast(uint32_t curr, uint32_t compare) {
    if ((curr >= compare) && ((curr - compare) < (1 << 30))) {  // "normal" check, disallowing case where compare wraps around
      return true;
    } else if ((curr < compare) && ((compare - curr) >= (1 << 30))) {  // overflow case where curr wraps around
      return true;
    } else {
      return false;
    }
  }

protected:
  Timer& usTimer_;

  atomic<uint8_t> currentIndex_;  // points to the state index that is safe to read from
  atomic<uint32_t> lastUs_[2];  // last usTimer read_us result on update
  atomic<uint32_t> usRollovers_[2];  // number of times usTimer has rolled over, essentially the high 32-bit word of a 64-bit us counter
};

/**
 * A ticker that can be polled for expiration. Long-term stable.
 */
class TimerTicker {
public:
  TimerTicker(uint32_t period_us, Timer& timer) :
      period_(period_us), timer_(timer) {
    tickerExpire_ = timer.read_us() + period_;
  }

  /**
   * Returns if the ticker has expired.
   * "Resets" the ticker on expiration.
   */
  bool checkExpired();

  /**
   * Reset the timer to start a period at the current time.
   */
  void reset();

protected:
  const uint32_t period_;  // period of ticker, in us

  Timer& timer_;  // reference to timebase
  uint32_t tickerExpire_;  // 32-bit time, in us, at which ticker is set to expire
};

#endif
