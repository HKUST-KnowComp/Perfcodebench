#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float clamp_sum_simd(const float* data, std::size_t n) {
  const ScalableTag<float> d;
  const size_t lanes = Lanes(d);
  auto v_sum = Zero(d);
  const auto v_zero = Zero(d);

  size_t i = 0;
  for (; i + lanes <= n; i += lanes) {
    const auto v = Load(d, data + i);
    v_sum = Add(v_sum, Max(v, v_zero));
  }

  float sum = ReduceSum(d, v_sum);
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
  return hwy::HWY_NAMESPACE::clamp_sum_simd(data, n);
}