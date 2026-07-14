#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace HWY =
    hwy::HWY_NAMESPACE;
}

float dot_product(const float* a, const float* b, std::size_t n) {
  if (n == 0) return 0.0f;

  const HWY::ScalableTag<float> d; // Use scalable tags for portability
  const size_t lanes = HWY::Lanes(d);
  
  // We use a loop that processes 'lanes' elements at a time.
  // To avoid precision issues with extremely large sums, we accumulate in SIMD registers.
  HWY::Simd<float> sum_vec = HWY::Zero(d);

  size_t i = 0;
  // Main SIMD loop
  for (; i + lanes <= n; i += lanes) {
    HWY::Simd<float> va = HWY::Load(d, a + i);
    HWY::Simd<float> vb = HWY::Load(d, b + i);
    sum_vec = HWY::Add(sum_vec, HWY::Mul(va, vb));
  }

  // Horizontal sum of the SIMD register
  float total_sum = HWY::ReduceSum(d, sum_vec);

  // Tail loop for remaining elements
  for (; i < n; ++i) {
    total_sum += a[i] * b[i];
  }

  return total_sum;
}