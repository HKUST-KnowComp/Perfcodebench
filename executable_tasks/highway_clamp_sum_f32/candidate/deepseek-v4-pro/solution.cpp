#include "interface.h"
#include <hwy/highway.h>

float clamp_sum(const float* data, std::size_t n) {
  if (n == 0) return 0.0f;
  namespace hn = hwy::HWY_NAMESPACE;
  const hn::ScalableTag<float> df;
  using Vec = hn::Vec<decltype(df)>;
  const size_t N = hn::Lanes(df);
  
  // Use 4 accumulators for instruction-level parallelism.
  constexpr int kNumAcc = 4;
  Vec acc[kNumAcc];
  for (int i = 0; i < kNumAcc; ++i) acc[i] = hn::Zero(df);
  
  size_t i = 0;
  // Process blocks of kNumAcc * N elements.
  for (; i + kNumAcc * N <= n; i += kNumAcc * N) {
    for (int j = 0; j < kNumAcc; ++j) {
      Vec v = hn::LoadU(df, data + i + j * N);
      v = hn::Max(v, hn::Zero(df));
      acc[j] = hn::Add(acc[j], v);
    }
  }
  // Process remaining full vectors.
  for (; i + N <= n; i += N) {
    Vec v = hn::LoadU(df, data + i);
    v = hn::Max(v, hn::Zero(df));
    acc[0] = hn::Add(acc[0], v);
  }
  
  // Combine accumulators and reduce.
  Vec sumv = hn::Zero(df);
  for (int j = 0; j < kNumAcc; ++j) {
    sumv = hn::Add(sumv, acc[j]);
  }
  float sum = hn::ReduceSum(df, sumv);
  
  // Process tail scalar.
  for (; i < n; ++i) {
    sum += data[i] > 0.0f ? data[i] : 0.0f;
  }
  return sum;
}