#include "interface.h"
#include <hwy/highway.h>

float clamp_sum(const float* data, std::size_t n) {
  const hwy::HWY_NAMESPACE::ScalableTag<float> d;
  auto sum_vec = hwy::SetZero(d);
  const size_t lanes = hwy::Lanes(d);
  for (std::size_t i = 0; i < n; i += lanes) {
    const auto mask = hwy::FirstN(d, n - i);
    const auto current = hwy::LoadMasked(mask, data + i);
    const auto clamped = hwy::Max(d, current, hwy::Zero(d));
    sum_vec = hwy::Add(d, sum_vec, clamped);
  }
  return hwy::ReduceSum(d, sum_vec);
}