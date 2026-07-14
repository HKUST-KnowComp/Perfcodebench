#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

static inline float ClampSumHWY(const float* HWY_RESTRICT data, size_t n) {
  const ScalableTag<float> d;
  using V = Vec<decltype(d)>;

  const size_t L = Lanes(d);
  V sum0 = Zero(d);
  V sum1 = Zero(d);
  const V zero = Zero(d);

  size_t i = 0;
  for (; i + 2 * L <= n; i += 2 * L) {
    const V v0 = Max(Load(d, data + i), zero);
    const V v1 = Max(Load(d, data + i + L), zero);
    sum0 = Add(sum0, v0);
    sum1 = Add(sum1, v1);
  }
  for (; i + L <= n; i += L) {
    const V v = Max(Load(d, data + i), zero);
    sum0 = Add(sum0, v);
  }

  sum0 = Add(sum0, sum1);
  float sum = ReduceSum(d, sum0);

  for (; i < n; ++i) {
    const float x = data[i];
    sum += x > 0.0f ? x : 0.0f;
    }

  return sum;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy
HWY_AFTER_NAMESPACE();

float clamp_sum(const float* data, std::size_t n) {
  return hwy::HWY_DYNAMIC_DISPATCH(ClampSumHWY)(data, n);
}
