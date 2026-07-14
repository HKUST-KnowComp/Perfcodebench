#include "interface.h"
#include <hwy/highway.h>

namespace hn = hwy::HWY_NAMESPACE;

/**
 * Optimized dot product implementation using the Highway SIMD library.
 * This implementation leverages SIMD registers to process multiple float elements
 * simultaneously, using Fused Multiply-Add (FMA) instructions where available
 * to maximize throughput and maintain precision.
 */
float dot_product(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  const std::size_t lanes = hn::Lanes(d);
  
  // Initialize SIMD accumulator to zero
  auto sum_v = hn::Zero(d);
  
  std::size_t i = 0;
  
  // Process elements in chunks of vector width
  // Unrolling is kept minimal to favor 'small-vector' performance
  if (n >= lanes) {
    for (; i <= n - lanes; i += lanes) {
      const auto va = hn::LoadU(d, a + i);
      const auto vb = hn::LoadU(d, b + i);
      sum_v = hn::MulAdd(va, vb, sum_v);
    }
  }

  // Reduce the SIMD vector to a single scalar sum
  float sum = hn::ReduceSum(d, sum_v);

  // Handle remaining elements (tail) with a scalar loop
  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }

  return sum;
}