#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float clamp_sum(const float* data, std::size_t n) {
  const ScalableTag<float> d;
  const auto zero = Zero(d);
  auto sum_v = zero;
  size_t i = 0;

  for (; i + Lanes(d) <= n; i += Lanes(d)) {
    const auto v = Load(d, data + i);
    const auto clamped = Max(zero, v);
    sum_v = Add(sum_v, clamped);
  }

  float sum = ReduceSum(d, sum_v);
  for (; i < n; ++i) {
    if (data[i] > 0.0f) {
      sum += data[i];
    }
  }
  return sum;
}

} // namespace HWY_NAMESPACE
} // namespace hwy
HWY_AFTER_NAMESPACE();

float clamp_sum(const float* data, std::size_t n) {
  return hwy::HWY_NAMESPACE::clamp_sum(data, n);
}