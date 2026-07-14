#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace HWY =
    hwy::HWY_NAMESPACE;
}

float clamp_sum(const float* data, std::size_t n) {
  if (n == 0) return 0.0f;

  const HWY::ScalableTag<float> d; 
  const size_t lanes = HWY::Lanes(d);
  const size_t loop_size = (n / lanes) * lanes;

  HWY::Simd<float> sum_vec = HWY::SetZero(d);
  const float zero = 0.0f;
  HWY::Simd<float> zero_vec = HWY::Set(d, zero);

  std::size_t i = 0;
  for (; i < loop_size; i += lanes) {
    HWY::Simd<float> v = HWY::Load(d, data + i);
    // Clamp: max(v, 0.0f)
    HWY::Simd<float> clamped = HWY::Max(v, zero_vec);
    sum_vec = HWY::Add(sum_vec, clamped);
  }

  float total_sum = HWY::ReduceSum(d, sum_vec);

  // Handle remaining elements
  for (; i < n; ++i) {
    float val = data[i];
    if (val > 0.0f) {
      total_sum += val;
    }
  }

  return total_sum;
}