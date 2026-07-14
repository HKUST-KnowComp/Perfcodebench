#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const auto zero = hn::Zero(d);
  auto sum_vec = hn::Zero(d);
  
  const std::size_t lanes = hn::Lanes(d);
  std::size_t i = 0;
  
  // Process full vectors
  for (; i + lanes <= n; i += lanes) {
    auto v = hn::LoadU(d, data + i);
    auto clamped = hn::Max(v, zero);
    sum_vec = hn::Add(sum_vec, clamped);
  }
  
  // Horizontal reduction
  float sum = hn::ReduceSum(d, sum_vec);
  
  // Process remaining elements
  for (; i < n; ++i) {
    sum += data[i] > 0.0f ? data[i] : 0.0f;
  }
  
  return sum;
}