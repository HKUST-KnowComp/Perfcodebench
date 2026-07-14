#include "interface.h"
#include "hwy/highway.h"

float dot_product(const float* a, const float* b, std::size_t n) {
  if (n == 0) return 0.0f;

  // Fast path for very small sizes to avoid SIMD setup/reduction overhead
  if (n <= 16) {
    float s = 0.0f;
    std::size_t i = 0;
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      s += a[i] * b[i];
      s += a[i + 1] * b[i + 1];
      s += a[i + 2] * b[i + 2];
      s += a[i + 3] * b[i + 3];
    }
    for (; i < n; ++i) s += a[i] * b[i];
    return s;
  }

  const float* HWY_RESTRICT pa = a;
  const float* HWY_RESTRICT pb = b;

  const hwy::HWY_FULL(float) df;
  auto sum0 = hwy::Zero(df);
  auto sum1 = hwy::Zero(df);

  std::size_t i = 0;
  const std::size_t L = hwy::Lanes(df);

  // Unroll by 2 to increase ILP and reduce dependency chains
  for (; i + 2 * L <= n; i += 2 * L) {
    const auto a0 = hwy::LoadU(df, pa + i);
    const auto b0 = hwy::LoadU(df, pb + i);
    const auto a1 = hwy::LoadU(df, pa + i + L);
    const auto b1 = hwy::LoadU(df, pb + i + L);
    sum0 = hwy::MulAdd(a0, b0, sum0);
    sum1 = hwy::MulAdd(a1, b1, sum1);
  }

  sum0 = hwy::Add(sum0, sum1);

  for (; i + L <= n; i += L) {
    const auto va = hwy::LoadU(df, pa + i);
    const auto vb = hwy::LoadU(df, pb + i);
    sum0 = hwy::MulAdd(va, vb, sum0);
  }

  float total = hwy::GetLane(hwy::SumOfLanes(df, sum0));

  for (; i < n; ++i) total += pa[i] * pb[i];

  return total;
}
