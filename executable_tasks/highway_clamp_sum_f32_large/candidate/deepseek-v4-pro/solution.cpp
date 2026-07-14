#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace project {
namespace HWY_NAMESPACE {

float clamp_sum_impl(const float* HWY_RESTRICT data, const std::size_t n) {
  const hn::ScalableTag<float> d;
  const std::size_t N = hn::Lanes(d);
  const auto zero = hn::Zero(d);
  auto sum = zero;

  std::size_t i = 0;
  for (; i + N <= n; i += N) {
    const auto v = hn::LoadU(d, data + i);
    const auto clamped = hn::Max(v, zero);
    sum = hn::Add(sum, clamped);
  }

  if (i < n) {
    const auto mask = hn::FirstN(d, n - i);
    const auto v = hn::MaskedLoad(mask, d, data + i);
    const auto clamped = hn::Max(v, zero);
    // Accumulate only active lanes without affecting other lanes
    sum = hn::MaskedAdd(sum, mask, sum, clamped);
  }

  return hn::ReduceSum(d, sum);
}

}  // namespace HWY_NAMESPACE
}  // namespace project
HWY_AFTER_NAMESPACE();

float clamp_sum(const float* data, std::size_t n) {
  return HWY_STATIC_DISPATCH(project::clamp_sum_impl)(data, n);
}