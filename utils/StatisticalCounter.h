#ifndef _STATICSTICAL_COUNTER_H
#define _STATICSTICAL_COUNTER_H

// Bisection-based integer square root
// https://stackoverflow.com/questions/1100090/looking-for-an-efficient-integer-square-root-algorithm-for-arm-thumb2
// Modification: this shrinks the hi and lo range each cycle to ensure some
// forward progress is always done, eliminating infinite loops.
template <typename T, typename V>
T fisqrt(V val) {
  V hi = val;
  V lo = 0;
  V mid = (hi + lo) / 2;
  V mid2 = mid * mid;
  while (lo < hi-1 && mid2 != val) {
    if (mid2 < val) {
      lo = mid;
      hi -= 1;
    } else {
      lo += 1;
      hi = mid;
    }
    mid = (hi + lo) / 2;
    mid2 = mid * mid;
  }
  // Do some fix-up work, walk to find the exact integer square root.
  if (mid2 < val) {
    while (mid2 < val) {
      mid += 1;
      mid2 = mid * mid;
    }
    return mid - 1;
  } else if (mid2 > val) {
    while (mid2 > val) {
      mid -= 1;
      mid2 = mid * mid;
    }
    return mid;
  } else {  // spot on!
    return mid;
  }
}

/**
 * Statistical counter, calculating the min, max, average, stdev of a collection
 * of samples added over time in an efficient manner.
 *
 * @tparam T type of individual data
 * @tparam V type of sum and sum-of-squares, which can be larger for accumulation
 */
template <typename T, typename V>
class StatisticalCounter {
public:
  struct StatisticalResult {
    size_t numSamples;
    T min;
    T max;
    T avg;
    T stdev;
  };

  StatisticalCounter() :
    numSamples_(0), min_(0), max_(0),
    sum_(0), squaredSum_(0) {
  }

  void addSample(T sample) {
    if (numSamples_ == 0) {
      min_ = sample;
      max_ = sample;
    } else {
      if (sample < min_) {
        min_ = sample;
      }
      if (sample > max_) {
        max_ = sample;
      }
    }
    sum_ += sample;
    squaredSum_ += (V)sample * sample;

    numSamples_++;
  }

  StatisticalResult read() const {
    StatisticalResult result;
    result.numSamples = numSamples_;
    result.min = min_;
    result.max = max_;
    if (numSamples_ > 0) {
      V avg = (sum_ + numSamples_ / 2) / numSamples_;  // rounding average
      result.avg = avg;
      V avgSq = (squaredSum_ + numSamples_ / 2) / numSamples_;
      V sqAvg = avg * avg;
      if (sqAvg >= avgSq) {
        result.stdev = 0;
      } else {
        result.stdev = fisqrt<T, V>(avgSq - sqAvg);
      }

    } else {
      result.avg = 0;
      result.stdev = 0;
    }
    return result;
  }

  void reset() {
    numSamples_ = 0;
    min_ = 0;
    max_ = 0;
    sum_ = 0;
    squaredSum_ = 0;
  }

protected:
  size_t numSamples_;
  T min_;
  T max_;

  V sum_;
  V squaredSum_;
};

#endif
