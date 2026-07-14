#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

static float ClampSumHWY(const float* HWY_RESTRICT data, size_t n) {
  const ScalableTag<float> d;
  const size_t L = Lanes(d);

  size_t i = 0;
  const auto zero = Zero(d);

  Vec<decltype(d)> sum0 = Zero(d);
  Vec<decltype(d)> sum1 = Zero(d);
  Vec<decltype(d)> sum2 = Zero(d);
  Vec<decltype(d)> sum3 = Zero(d);

  for (; i + 4 * L <= n; i += 4 * L) {
    const auto v0 = Max(LoadU(d, data + i + 0 * L), zero);
    const auto v1 = Max(LoadU(d, data + i + 1 * L), zero);
    const auto v2 = Max(LoadU(d, data + i + 2 * L), zero);
    const auto v3 = Max(LoadU(d, data + i + 3 * L), zero);
    sum0 = Add(sum0, v0);
    sum1 = Add(sum1, v1);
    sum2 = Add(sum2, v2);
    sum3 = Add(sum3, v3);
  }

  Vec<decltype(d)> sum = Add(Add(sum0, sum1), Add(sum2, sum3));

  for (; i + L <= n; i += L) {
    const auto v = Max(LoadU(d, data + i), zero);
    sum = Add(sum, v);
  }

  float total = GetLane(SumOfLanes(d, sum));

  for (; i < n; ++i) {
    const float v = data[i];
    if (v > 0.0f) total += v;
  }

  return total;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy
HWY_AFTER_NAMESPACE();

float clamp_sum(const float* data, std::size_t n) {
  return hwy::HWY_DYNAMIC_DISPATCH(ClampSumHWY)(data, n);
}
