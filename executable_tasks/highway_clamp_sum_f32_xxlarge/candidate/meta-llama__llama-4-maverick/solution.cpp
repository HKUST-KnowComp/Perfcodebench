#include "interface.h"
#include "hwy/highway.h"

float clamp_sum(const float* data, std::size_t n) {
  namespace hn = hwy::HWY_NAMESPACE;
  using hwy::float32_t = hn::TFromD<hwy::N::Simd<float, 0, 0>>;
  using hwy::N::ZeroIfNegative;
  using hwy::N::Mul;
  using hwy::N::SumOfLanes;
  using hwy::N::LoadU;

  float sum = 0.0f;
  const hwy::N::ScalableTag<float> d;
  const auto zero = hn::Zero(d);
  const size_t N = hn::Lanes(d);
  size_t i = 0;
  auto sum_vec = zero;
  for (; i + N <= n; i += N) {
    const auto loaded = LoadU(d, data + i);
    const auto clamped = ZeroIfNegative(loaded);
    sum_vec = Mul(sum_vec, loaded > zero, loaded);
    sum_vec = hn::Add(sum_vec, clamped);
  }
  sum = SumOfLanes(d, sum_vec);
  for (; i < n; ++i) {
    sum += data[i] > 0.0f ? data[i] : 0.0f;
  }
  return sum;
}