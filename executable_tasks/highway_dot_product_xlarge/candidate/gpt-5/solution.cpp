#include "interface.h"
#include "hwy/highway.h"

float dot_product(const float* a, const float* b, std::size_t n) {
  namespace hn = hwy::HWY_NAMESPACE;
  const hn::ScalableTag<float> d;
  auto vsum = hn::Zero(d);

  std::size_t i = 0;
  const std::size_t L = hn::Lanes(d);

  // Unroll by 2 for better throughput
  for (; i + 2 * L <= n; i += 2 * L) {
    const auto va0 = hn::LoadU(d, a + i);
    const auto vb0 = hn::LoadU(d, b + i);
    const auto va1 = hn::LoadU(d, a + i + L);
    const auto vb1 = hn::LoadU(d, b + i + L);
    vsum = hn::MulAdd(va0, vb0, vsum);
    vsum = hn::MulAdd(va1, vb1, vsum);
  }

  for (; i + L <= n; i += L) {
    const auto va = hn::LoadU(d, a + i);
    const auto vb = hn::LoadU(d, b + i);
    vsum = hn::MulAdd(va, vb, vsum);
  }

  float total = hn::GetLane(hn::SumOfLanes(vsum));

  // Remainder
  for (; i < n; ++i) {
    total += a[i] * b[i];
  }

  return total;
}
