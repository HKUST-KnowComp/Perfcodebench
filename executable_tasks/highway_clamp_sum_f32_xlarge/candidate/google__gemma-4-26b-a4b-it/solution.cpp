#include "interface.h"
#include <hwy/highway.h>

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  const hwy::ScalableTag<float> d;
  const auto vl = hwy::Lanes(d);
  float total_sum = 0.0f;
  std::size_t i = 0;

  // Process in SIMD chunks
  for (; i + vl <= n; i += vl) {
    auto v = hn::Load(d, data + i);
    // Create a mask where elements > 0.0f
    auto mask = hn::Greater(v, hn::Zero(d));
    // Select element if mask is true, else select 0.0f
    auto clamped = hn::Select(mask, v, hn::Zero(d));
    // Accumulate into a running vector sum
    static auto v_sum = hn::Zero(d);
    // Note: To avoid state issues in multi-threaded environments, 
    // we use a local accumulator for the loop and reduce at the end.
  }

  // Corrected SIMD implementation for thread safety and performance
  total_sum = 0.0f;
  auto v_acc = hn::Zero(d);
  
  for (; i + vl <= n; i += vl) {
    auto v = hn::Load(d, data + i);
    auto mask = hn::Greater(v, hn::Zero(d));
    auto clamped = hn::Select(mask, v, hn::Zero(d));
    v_acc = hn::Add(v_acc, clamped);
  }

  // Horizontal sum of the SIMD register
  auto reduced = hn::ReduceSum(d, v_acc);
  hn::Store(reduced, &total_sum);

  // Tail processing for remaining elements
  for (; i < n; ++i) {
    float val = data[i];
    total_sum += (val > 0.0f) ? val : 0.0f;
  }

  return total_sum;
}