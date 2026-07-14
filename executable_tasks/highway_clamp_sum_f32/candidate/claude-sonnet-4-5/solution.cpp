#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const size_t N = hn::Lanes(d);
  
  auto sum_vec = hn::Zero(d);
  const auto zero = hn::Zero(d);
  
  size_t i = 0;
  for (; i + N <= n; i += N) {
    auto v = hn::LoadU(d, data + i);
    v = hn::Max(v, zero);
    sum_vec = hn::Add(sum_vec, v);
  }
  
  float sum = hn::ReduceSum(d, sum_vec);
  
  for (; i < n; ++i) {
    sum += data[i] > 0.0f ? data[i] : 0.0f;
  }
  
  return sum;
}