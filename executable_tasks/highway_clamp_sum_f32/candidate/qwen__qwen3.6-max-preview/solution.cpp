#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

float ClampSumImpl(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const auto zero = hn::Zero(d);
  auto sum_vec = zero;
  const size_t N = hn::Lanes(d);
  std::size_t i = 0;
  for (; i + N <= n; i += N) {
    auto v = hn::LoadU(d, data + i);
    sum_vec = hn::Add(sum_vec, hn::Max(v, zero));
  }
  float sum = hn::ReduceSum(d, sum_vec);
  for (; i < n; ++i) {
    float val = data[i];
    sum += val > 0.0f ? val : 0.0f;
  }
  return sum;
}

} // namespace HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace HWY_NAMESPACE {
HWY_EXPORT(ClampSumImpl);
}

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(ClampSumImpl)(data, n);
}
#endif
