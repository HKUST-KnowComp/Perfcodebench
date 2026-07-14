#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace hn = hwy::HWY_NAMESPACE;

float DotScalar(const float* a, const float* b, std::size_t n) {
  float sum = 0.0f;
  for (std::size_t i = 0; i < n; ++i) {
    sum += a[i] * b[i];
  }
  return sum;
}
}  // namespace

float dot_product(const float* a, const float* b, std::size_t n) {
  if (n == 0) return 0.0f;

  const hn::ScalableTag<float> d;
  const std::size_t lanes = hn::Lanes(d);

  // For very small inputs, scalar avoids SIMD setup overhead.
  if (n < lanes) {
    return DotScalar(a, b, n);
  }

  std::size_t i = 0;
  auto acc0 = hn::Zero(d);
  auto acc1 = hn::Zero(d);

  // Unroll by 2 to improve throughput on small/medium vectors while
  // keeping code size modest.
  const std::size_t step = 2 * lanes;
  for (; i + step <= n; i += step) {
    const auto va0 = hn::LoadU(d, a + i);
    const auto vb0 = hn::LoadU(d, b + i);
    const auto va1 = hn::LoadU(d, a + i + lanes);
    const auto vb1 = hn::LoadU(d, b + i + lanes);
    acc0 = hn::MulAdd(va0, vb0, acc0);
    acc1 = hn::MulAdd(va1, vb1, acc1);
  }

  auto acc = acc0 + acc1;

  for (; i + lanes <= n; i += lanes) {
    const auto va = hn::LoadU(d, a + i);
    const auto vb = hn::LoadU(d, b + i);
    acc = hn::MulAdd(va, vb, acc);
  }

  float sum = hn::GetLane(hn::SumOfLanes(d, acc));

  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }

  return sum;
}
