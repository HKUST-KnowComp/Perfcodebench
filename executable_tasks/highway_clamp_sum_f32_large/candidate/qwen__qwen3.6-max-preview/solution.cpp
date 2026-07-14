#include "hwy/highway.h"
#include "interface.h"

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum_impl(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const size_t N = hn::Lanes(d);
  auto sum_vec = hn::Zero(d);
  size_t i = 0;
  for (; i + N <= n; i += N) {
    auto v = hn::LoadU(d, data + i);
    v = hn::Max(v, hn::Zero(d));
    sum_vec = hn::Add(sum_vec, v);
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
HWY_EXPORT(clamp_sum_impl);
}

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(clamp_sum_impl)(data, n);
}
#endif