#include "interface.h"
#include <hwy/highway.h>

float clamp_sum(const float* data, std::size_t n) {
  namespace hn = hwy::HWY_NAMESPACE;
  const hn::ScalableTag<float> d;
  const size_t num_lanes = hn::Lanes(d);
  hn::Vec<decltype(d)> sum_vec = hn::Zero(d);

  // Process full SIMD-aligned chunks of input
  size_t i = 0;
  for (; i + num_lanes <= n; i += num_lanes) {
    const auto vec = hn::Load(d, data + i);
    const auto clamped = hn::Max(vec, hn::Zero(d));
    sum_vec = hn::Add(sum_vec, clamped);
  }

  // Reduce vector sum to scalar, then process remaining elements
  float total = hn::ReduceSum(d, sum_vec);
  for (; i < n; ++i) {
    if (data[i] > 0.0f) total += data[i];
  }

  return total;
}