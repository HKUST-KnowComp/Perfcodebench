#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float clamp_sum(const float* data, std::size_t n) {
  const ScalableTag<float> d;
  auto sum_v = Zero(d);
  size_t i = 0;
  const auto zero_v = Zero(d);

  for (; i + Lanes(d) <= n; i += Lanes(d)) {
    const auto v = Load(d, data + i);
    // Clamp negative values to zero: max(0, v)
    const auto clamped = Max(zero_v, v);
    sum_v = Add(sum_v, clamped);
  }

  float sum = ReduceSum(d, sum_v);
  for (; i < n; ++i) {
    sum += (data[i] > 0.0f ? data[i] : 0.0f);
  }
  return sum;
}

} // namespace HWY_NAMESPACE
} // namespace hwy
HWY_AFTER_NAMESPACE();

float clamp_sum(const float* data, std::size_t n) {
  return hwy::HWY_NAMESPACE::clamp_sum(data, n);
}