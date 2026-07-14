#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

static float ClampSumHWY(const float* data, std::size_t n) {
  const hwy::ScalableTag<float> d;
  const std::size_t L = hwy::Lanes(d);

  std::size_t i = 0;
  auto sum0 = hwy::Zero(d);
  auto sum1 = hwy::Zero(d);
  const auto zero = hwy::Zero(d);

  // Process two vectors per iteration to reduce the add dependency chain
  for (; i + 2 * L <= n; i += 2 * L) {
    auto v0 = hwy::LoadU(d, data + i);
    auto v1 = hwy::LoadU(d, data + i + L);
    v0 = hwy::Max(v0, zero);
    v1 = hwy::Max(v1, zero);
    sum0 = hwy::Add(sum0, v0);
    sum1 = hwy::Add(sum1, v1);
  }

  sum0 = hwy::Add(sum0, sum1);

  // One more full vector if available
  for (; i + L <= n; i += L) {
    auto v = hwy::LoadU(d, data + i);
    v = hwy::Max(v, zero);
    sum0 = hwy::Add(sum0, v);
  }

  float sum = hwy::ReduceSum(d, sum0);

  // Remainder
  for (; i < n; ++i) {
    const float v = data[i];
    if (v > 0.0f) sum += v;
  }

  return sum;
}

}  // namespace HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(ClampSumHWY)(data, n);
}
