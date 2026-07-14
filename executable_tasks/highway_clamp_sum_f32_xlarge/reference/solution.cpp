#include "interface.h"

#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hn = hwy::HWY_NAMESPACE;
namespace hwy {
namespace HWY_NAMESPACE {

HWY_ATTR float ClampSumImpl(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const auto zero = hn::Zero(d);
  auto sum = hn::Zero(d);
  std::size_t i = 0;
  for (; i + hn::Lanes(d) <= n; i += hn::Lanes(d)) {
    const auto v = hn::LoadU(d, data + i);
    sum = hn::Add(sum, hn::Max(v, zero));
  }
  float out = hn::ReduceSum(d, sum);
  for (; i < n; ++i) {
    out += data[i] > 0.0f ? data[i] : 0.0f;
  }
  return out;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy
HWY_AFTER_NAMESPACE();

float clamp_sum(const float* data, std::size_t n) {
  return hwy::HWY_NAMESPACE::ClampSumImpl(data, n);
}
