#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum_impl(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const size_t N = hn::Lanes(d);
  auto sum0 = hn::Zero(d);
  auto sum1 = hn::Zero(d);
  auto sum2 = hn::Zero(d);
  auto sum3 = hn::Zero(d);
  const auto zero = hn::Zero(d);

  size_t i = 0;
  for (; i + 4 * N <= n; i += 4 * N) {
    auto v0 = hn::Load(d, data + i);
    auto v1 = hn::Load(d, data + i + N);
    auto v2 = hn::Load(d, data + i + 2 * N);
    auto v3 = hn::Load(d, data + i + 3 * N);
    sum0 = hn::Add(sum0, hn::Max(v0, zero));
    sum1 = hn::Add(sum1, hn::Max(v1, zero));
    sum2 = hn::Add(sum2, hn::Max(v2, zero));
    sum3 = hn::Add(sum3, hn::Max(v3, zero));
  }

  sum0 = hn::Add(hn::Add(sum0, sum1), hn::Add(sum2, sum3));
  float sum = hn::ReduceSum(d, sum0);

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
HWY_EXPORT(clamp_sum_impl);
}

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(clamp_sum_impl)(data, n);
}
#endif