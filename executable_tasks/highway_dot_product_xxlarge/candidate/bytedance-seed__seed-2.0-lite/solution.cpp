#include "interface.h"
#include <hwy/highway.h>

float dot_product(const float* restrict a, const float* restrict b, std::size_t n) {
  using namespace hwy;
  ScalableTag<float> tag;
  const size_t L = Lanes(tag);

  // 4 vector accumulators to hide add latency and saturate memory bandwidth
  Vec<decltype(tag)> sum0 = Zero(tag);
  Vec<decltype(tag)> sum1 = Zero(tag);
  Vec<decltype(tag)> sum2 = Zero(tag);
  Vec<decltype(tag)> sum3 = Zero(tag);

  size_t i = 0;
  // Main loop: process 4 full vector blocks per iteration for large inputs
  for (; i + 4 * L <= n; i += 4 * L) {
    auto va0 = Load(tag, a + i);
    auto vb0 = Load(tag, b + i);
    sum0 = MulAdd(va0, vb0, sum0);

    auto va1 = Load(tag, a + i + L);
    auto vb1 = Load(tag, b + i + L);
    sum1 = MulAdd(va1, vb1, sum1);

    auto va2 = Load(tag, a + i + 2*L);
    auto vb2 = Load(tag, b + i + 2*L);
    sum2 = MulAdd(va2, vb2, sum2);

    auto va3 = Load(tag, a + i + 3*L);
    auto vb3 = Load(tag, b + i + 3*L);
    sum3 = MulAdd(va3, vb3, sum3);
  }

  // Process remaining full vector blocks after main loop
  for (; i + L <= n; i += L) {
    auto va = Load(tag, a + i);
    auto vb = Load(tag, b + i);
    sum0 = MulAdd(va, vb, sum0);
  }

  // Process any leftover partial vector with masked load to avoid out-of-bounds access
  if (i < n) {
    const auto mask = FirstN(tag, n - i);
    auto va = LoadMasked(tag, mask, a + i);
    auto vb = LoadMasked(tag, mask, b + i);
    sum0 = MulAdd(va, vb, sum0);
  }

  // Combine all accumulators
  sum0 = Add(sum0, sum1);
  sum2 = Add(sum2, sum3);
  sum0 = Add(sum0, sum2);

  // Reduce vector sum to final scalar dot product
  return ReduceSum(tag, sum0);
}