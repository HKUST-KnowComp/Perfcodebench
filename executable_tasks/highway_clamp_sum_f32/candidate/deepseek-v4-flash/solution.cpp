#include "interface.h"
#include "hwy/highway.h"

namespace hwy {
namespace HWY_NAMESPACE {
float clamp_sum(const float* data, std::size_t n) {
  using D = ScalableTag<float>;
  D d;
  auto sumv = Zero(d);
  size_t i = 0;
  for (; i + Lanes(d) <= n; i += Lanes(d)) {
    auto v = LoadU(d, data + i);
    sumv = Add(sumv, Max(v, Zero(d)));
  }
  float sum = ReduceSum(d, sumv);
  for (; i < n; ++i) {
    sum += data[i] > 0.0f ? data[i] : 0.0f;
  }
  return sum;
}
} // namespace HWY_NAMESPACE
} // namespace hwy

HWY_EXPORT(clamp_sum);

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(clamp_sum)(data, n);
}
