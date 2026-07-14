#include "interface.h"
#include <hwy/highway.h>

float clamp_sum(const float* data, std::size_t n) {
  const hwy::HWY_NAMESPACE::ScalableTag<float> d;
  const size_t lanes = hwy::Lanes(d);
  auto sum_vec = hwy::Zero(d);

  // Process full SIMD vector chunks
  for (std::size_t i = 0; i + lanes <= n; i += lanes) {
    const auto v = hwy::LoadU(d, data + i);
    const auto clamped = hwy::Max(v, hwy::Zero(d));
    sum_vec = hwy::Add(sum_vec, clamped);
  }

  // Process remaining partial vector if any
  const std::size_t rem = n % lanes;
  if (rem != 0) {
    const auto mask = hwy::FirstN(d, rem);
    const auto v = hwy::LoadMasked(d, mask, data + (n - rem));
    const auto clamped = hwy::Max(v, hwy::Zero(d));
    sum_vec = hwy::Add(sum_vec, clamped);
  }

  // Reduce vector sum to final scalar value
  return hwy::ReduceSum(d, sum_vec);
}