#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float ClampSumImpl(const float* data, std::size_t n) {
  const ScalableTag<float> d;
  const std::size_t lanes = Lanes(d);
  const Vec<decltype(d)> zero = Zero(d);

  std::size_t i = 0;

  // Multiple independent accumulators improve throughput on large inputs.
  Vec<decltype(d)> sum0 = Zero(d);
  Vec<decltype(d)> sum1 = Zero(d);
  Vec<decltype(d)> sum2 = Zero(d);
  Vec<decltype(d)> sum3 = Zero(d);

  const std::size_t unroll = 4 * lanes;
  for (; i + unroll <= n; i += unroll) {
    const Vec<decltype(d)> v0 = LoadU(d, data + i);
    const Vec<decltype(d)> v1 = LoadU(d, data + i + lanes);
    const Vec<decltype(d)> v2 = LoadU(d, data + i + 2 * lanes);
    const Vec<decltype(d)> v3 = LoadU(d, data + i + 3 * lanes);

    sum0 = Add(sum0, Max(v0, zero));
    sum1 = Add(sum1, Max(v1, zero));
    sum2 = Add(sum2, Max(v2, zero));
    sum3 = Add(sum3, Max(v3, zero));
  }

  Vec<decltype(d)> sum = Add(Add(sum0, sum1), Add(sum2, sum3));

  for (; i + lanes <= n; i += lanes) {
    const Vec<decltype(d)> v = LoadU(d, data + i);
    sum = Add(sum, Max(v, zero));
  }

  float result = GetLane(SumOfLanes(d, sum));

  for (; i < n; ++i) {
    const float x = data[i];
    result += x > 0.0f ? x : 0.0f;
  }

  return result;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
float clamp_sum(const float* data, std::size_t n) {
  return hwy::HWY_DYNAMIC_DISPATCH(ClampSumImpl)(data, n);
}
#endif
