#include "interface.h"
#include <hwy/highway.h>

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  using d_type = hn::ScalableTag<float>;
  d_type d;
  const std::size_t N = hn::Lanes(d);
  hn::VFromD<d_type> sum = hn::Zero(d);
  
  std::size_t i = 0;
  for (; i + N <= n; i += N) {
    auto v = hn::LoadU(d, data + i);
    auto mask = hn::Gt(v, hn::Zero(d));
    auto clamped = hn::IfThenElseZero(mask, v);
    sum = hn::Add(sum, clamped);
  }
  
  float partial = hn::GetLane(hn::SumOfLanes(d, sum));
  for (; i < n; ++i) {
    if (data[i] > 0.0f) partial += data[i];
  }
  return partial;
}
