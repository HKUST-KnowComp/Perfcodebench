#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const auto zero = hn::Zero(d);
  auto sumv = zero;

  std::size_t i = 0;
  const std::size_t N = hn::Lanes(d);
  for (; i + N <= n; i += N) {
    const auto v = hn::Load(d, data + i);
    const auto clamped = hn::ZeroIfNegative(v);
    sumv = hn::Add(sumv, clamped);
  }

  float sum = hn::ReduceSum(d, sumv);
  for (; i < n; ++i) {
    sum += data[i] > 0.0f ? data[i] : 0.0f;
  }
  return sum;
}