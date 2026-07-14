#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace HWY =
    hwy::HWY_NAMESPACE;
}

float clamp_sum(const float* data, std::size_t n) {
  if (n == 0) return 0.0f;

  const HWY::ScalableTag<float> d; // Use scalable tags for portability
  const size_t lanes = HWY::Lanes(d);
  const size_t loop_size = (n / lanes) * lanes;

  float total_sum = 0.0f;
  const float* ptr = data;

  // SIMD Loop
  for (std::size_t i = 0; i < loop_size; i += lanes) {
    auto v = HWY::Load(d, ptr + i);
    auto zero = HWY::Set(d, 0.0f);
    // Clamp: max(v, 0.0f)
    auto clamped = HWY::Max(v, zero);
    total_sum += HWY::ReduceSum(d, clamped);
  }

  // Tail Loop
  for (std::size_t i = loop_size; i < n; ++i) {
    float val = data[i];
    total_sum += (val > 0.0f) ? val : 0.0f;
  }

  return total_sum;
}