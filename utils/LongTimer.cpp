#include "LongTimer.h"

void LongTimer::update() {
  // NOT THREAD SAFE, only one thread may do updates.
  uint32_t currentUs = (uint32_t)usTimer_.read_us();

  uint8_t currentIndex = currentIndex_.load(std::memory_order_acquire);
  uint8_t nextIndex = (currentIndex + 1) % 2;

  uint32_t currentRollover = usRollovers_[currentIndex].load(std::memory_order_relaxed);
  if (currentUs < lastUs_[currentIndex]) {
    currentRollover += 1;
  }
  usRollovers_[nextIndex].store(currentRollover, std::memory_order_relaxed);
  lastUs_[nextIndex].store(currentUs, std::memory_order_relaxed);

  currentIndex_.store(nextIndex, std::memory_order_release);
}

uint32_t LongTimer::read_short_us() {
  return usTimer_.read_us();
}

uint64_t LongTimer::read_us() {
  // NOT THREAD SAFE, but is interrupt-safe with update.
  uint32_t currentUs = (uint32_t)usTimer_.read_us();
  uint8_t bufferedCurrentIndex = currentIndex_.load(std::memory_order_acquire);
  uint32_t currentRollover = usRollovers_[bufferedCurrentIndex].load(std::memory_order_relaxed);
  if (currentUs < lastUs_[bufferedCurrentIndex]) {
    currentRollover += 1;
  }
  return ((uint64_t)currentRollover << 32) | currentUs;
}

uint32_t LongTimer::read_ms() {
  return uint32_t(read_us() / 1000);
}

bool TimerTicker::checkExpired() {
  uint32_t currentUs = timer_.read_us();
  if (LongTimer::timePast(currentUs, tickerExpire_)) {
    tickerExpire_ += period_;
    return true;
  }
  return false;
}

void TimerTicker::reset() {
  tickerExpire_ = timer_.read_us() + period_;
}
