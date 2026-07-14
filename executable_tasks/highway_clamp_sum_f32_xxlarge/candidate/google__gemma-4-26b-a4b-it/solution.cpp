#include "interface.h"
#include "hwy/highway.h"

namespace {
namespace HWY =
the
hwy::HWY;
}

float clamp_sum(const float* data, std::size_t n) {
  if (n == 0) return 0.0f;

  const HWY::ScalableTag<float> d;
  const size_t lanes = HWY::Lanes(d);
  const size_t loop_size = (n / lanes) * lanes;

  float total_sum = 0.0f;
  const float* ptr = data;

  // SIMD Loop
  if (loop_size > 0) {
    HWY::Simd<float> sum_vec = HWY::SetZero(d);
    HWY::Simd<float> zero_vec = HWY::SetZero(d);

    for (std::size_t i = 0; i < loop_size; i += lanes) {
      HWY::Simd<float> v = HWY::Load(d, ptr + i);
      // Clamp: max(v, 0.0f)
      HWY::Simd<float> clamped = HWY::Max(v, zero_vec);
      sum_vec = HWY::Add(sum_vec, clamped);
    }
    total_sum = HWY::ReduceSum(d, sum_vec);
    ptr += loop_size;
  }

  // Scalar Tail
  for (std::size_t i = 0; i < (n - loop_size); ++i) {
    float val = ptr[i];
    if (val > 0.0f) {
      total_sum += val;
    }
  }

  return total_sum;
}