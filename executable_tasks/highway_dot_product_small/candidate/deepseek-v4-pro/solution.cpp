#include "interface.h"
#include <hwy/highway.h>

float dot_product(const float* a, const float* b, std::size_t n) {
  namespace HWY = hwy::HWY_NAMESPACE;
  using D = HWY::ScalableTag<float>;
  const D d;
  const size_t lanes = HWY::Lanes(d);
  auto sum = HWY::Zero(d);
  size_t i = 0;
  for (; i + lanes <= n; i += lanes) {
    const auto va = HWY::LoadU(d, a + i);
    const auto vb = HWY::LoadU(d, b + i);
    sum = HWY::MulAdd(va, vb, sum);
  }
  const auto sum_of_lanes = HWY::SumOfLanes(d, sum);
  float result = HWY::GetLane(sum_of_lanes);
  for (; i < n; ++i) {
    result += a[i] * b[i];
  }
  return result;
}