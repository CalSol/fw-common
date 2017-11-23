#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include <stddef.h>
#include <limits>

/**
 * Bucketing histogram, sorts every incoming sample into bucket counters, giving a coarse
 * distribution.
 *
 * @tparam NumDividers number of dividers between buckets
 * @tparam T type of bucket limits
 * @tparam V type of count
 */
template <size_t NumDividers, typename T, typename V>
class Histogram {
public:
  /**
   * Creates a new histogram object with set bucket dividers.
   * The elements of dividers indicate the exclusive upper limits of each bucket.
   * Bucket 0 spans the interval [T::min, bucketDividers[0])
   * Bucket i spans the interval [bucketDividers[i-1], bucketDividers[i])
   * The last bucket spans the interval [bucketDividers[n], T::max]
   * The last bucket is the one where the divider is the same or less than the previous divider.
   */
  Histogram(const T (&bucketDividers)[NumDividers]) :
      bucketCounts_{0} {
    T lastDivider = std::numeric_limits<T>::lowest();
    usedBuckets_ = NumDividers + 1;
    for (size_t i=0; i<NumDividers; i++) {
      if (bucketDividers[i] <= lastDivider) {
        usedBuckets_ = i + 1;
        break;
      }
      bucketDividers_[i] = bucketDividers[i];
    }
  }

  /**
   * Clears bucket counts.
   */
  void reset() {
    for (size_t i=0; i<usedBuckets_; i++) {
      bucketCounts_[i] = 0;
    }
  }

  void addSample(T sample) {
    size_t sampleBucket = usedBuckets_ - 1;
    for (size_t i=0; i<usedBuckets_-1; i++) {
      if (sample < bucketDividers_[i]) {
        sampleBucket = i;
        break;
      }
    }
    bucketCounts_[sampleBucket]++;
  }

  /**
   * Returns the histogram bucket dividers and counts.
   */
  size_t read(const T** dividersOut, const V** countsOut) {
    *dividersOut = bucketDividers_;
    *countsOut = bucketCounts_;
    return usedBuckets_;
  }

private:
  size_t usedBuckets_;  // number of bucketCounts_ in use, based on bucketDividers_
  // this really should be const, but plain arrays won't initialize from an initializer_list
  T bucketDividers_[NumDividers];  // exclusive upper limit of each bucket
  V bucketCounts_[NumDividers+1];

};

#endif
