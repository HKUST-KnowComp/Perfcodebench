#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hwy { namespace HWY_NAMESPACE {} }
HWY_AFTER_NAMESPACE();
using namespace hwy::HWY_NAMESPACE;

float dot_product(const float* a, const float* b, std::size_t n) {
  const ScalableTag<float> tag;
  const size_t simd_lanes = Lanes(tag);
  float total = 0.0f;
  size_t i = 0;

  // Process full SIMD chunks for aligned vectorized computation
  if (n >= simd_lanes) {
    auto accumulator = SetZero(tag);
    for (; i + simd_lanes <= n; i += simd_lanes) {
      const auto vec_a = LoadU(tag, a + i);
      const auto vec_b = LoadU(tag, b + i);
      accumulator = Add(accumulator, Mul(vec_a, vec_b));
    }
    total = SumOfLanes(tag, accumulator);
  }

  // Process any remaining elements that didn't fit in a full SIMD chunk
  for (; i < n; ++i) {
    total += a[i] * b[i];
  }

  return total;
}