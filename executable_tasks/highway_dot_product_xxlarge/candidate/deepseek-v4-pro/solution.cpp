#include "interface.h"
#include <hwy/highway.h>

namespace hn = hwy::HWY_NAMESPACE;

float dot_product(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  using V = decltype(hn::Zero(d));
  const size_t N = hn::Lanes(d);
  size_t i = 0;
  V sum0 = hn::Zero(d);
  V sum1 = hn::Zero(d);
  V sum2 = hn::Zero(d);
  V sum3 = hn::Zero(d);

  // Main loop: 4 vectors per iteration for better ILP
  for (; i + 4 * N <= n; i += 4 * N) {
    V a0 = hn::LoadU(d, a + i);
    V b0 = hn::LoadU(d, b + i);
    sum0 = hn::MulAdd(a0, b0, sum0);
    V a1 = hn::LoadU(d, a + i + N);
    V b1 = hn::LoadU(d, b + i + N);
    sum1 = hn::MulAdd(a1, b1, sum1);
    V a2 = hn::LoadU(d, a + i + 2 * N);
    V b2 = hn::LoadU(d, b + i + 2 * N);
    sum2 = hn::MulAdd(a2, b2, sum2);
    V a3 = hn::LoadU(d, a + i + 3 * N);
    V b3 = hn::LoadU(d, b + i + 3 * N);
    sum3 = hn::MulAdd(a3, b3, sum3);
  }

  // Handle remaining full vectors (unrolled accumulators continue)
  for (; i + N <= n; i += N) {
    V a = hn::LoadU(d, a + i);
    V b = hn::LoadU(d, b + i);
    sum0 = hn::MulAdd(a, b, sum0);
  }

  // Combine the four accumulators
  sum0 = hn::Add(hn::Add(sum0, sum1), hn::Add(sum2, sum3));
  float result = hn::ReduceSum(d, sum0);

  // Scalar tail
  for (; i < n; ++i) {
    result += a[i] * b[i];
  }

  return result;
}