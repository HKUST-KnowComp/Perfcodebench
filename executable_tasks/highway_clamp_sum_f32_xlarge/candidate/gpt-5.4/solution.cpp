#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float ClampSumImpl(const float* data, std::size_t n) {
  const ScalableTag<float> d;
  const auto zero = Zero(d);

  std::size_t i = 0;
  Vec<decltype(d)> vsum0 = Zero(d);
  Vec<decltype(d)> vsum1 = Zero(d);
  Vec<decltype(d)> vsum2 = Zero(d);
  Vec<decltype(d)> vsum3 = Zero(d);

  const std::size_t L = Lanes(d);
  const std::size_t step = 4 * L;
  const std::size_t limit4 = n & ~(step - 1);

  for (; i < limit4; i += step) {
    const auto v0 = LoadU(d, data + i);
    const auto v1 = LoadU(d, data + i + L);
    const auto v2 = LoadU(d, data + i + 2 * L);
    const auto v3 = LoadU(d, data + i + 3 * L);

    vsum0 = Add(vsum0, Max(v0, zero));
    vsum1 = Add(vsum1, Max(v1, zero));
    vsum2 = Add(vsum2, Max(v2, zero));
    vsum3 = Add(vsum3, Max(v3, zero));
  }

  Vec<decltype(d)> vsum = Add(Add(vsum0, vsum1), Add(vsum2, vsum3));

  const std::size_t limit = n & ~(L - 1);
  for (; i < limit; i += L) {
    const auto v = LoadU(d, data + i);
    vsum = Add(vsum, Max(v, zero));
  }

  float sum = GetLane(SumOfLanes(d, vsum));
  for (; i < n; ++i) {
    const float x = data[i];
    sum += x > 0.0f ? x : 0.0f;
  }
  return sum;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
HWY_EXPORT(ClampSumImpl);
#endif

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(ClampSumImpl)(data, n);
}
