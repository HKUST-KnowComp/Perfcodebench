#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {
}
} // namespace hwy
HWY_AFTER_NAMESPACE();

using namespace hwy;

float dot_product(const float* a, const float* b, std::size_t n) {
  if (n == 0) return 0.0f;
  const ScalableTag<float> tag;
  const size_t simd_lanes = Lanes(tag);
  const size_t block_size = 4 * simd_lanes;
  
  auto sum0 = Zero(tag);
  auto sum1 = Zero(tag);
  auto sum2 = Zero(tag);
  auto sum3 = Zero(tag);
  size_t i = 0;

  // Process large unrolled blocks to hide accumulation latency
  for (; i + block_size <= n; i += block_size) {
    auto va = Load(tag, a + i);
    auto vb = Load(tag, b + i);
    sum0 = MulAdd(va, vb, sum0);

    va = Load(tag, a + i + simd_lanes);
    vb = Load(tag, b + i + simd_lanes);
    sum1 = MulAdd(va, vb, sum1);

    va = Load(tag, a + i + 2*simd_lanes);
    vb = Load(tag, b + i + 2*simd_lanes);
    sum2 = MulAdd(va, vb, sum2);

    va = Load(tag, a + i + 3*simd_lanes);
    vb = Load(tag, b + i + 3*simd_lanes);
    sum3 = MulAdd(va, vb, sum3);
  }

  // Combine primary accumulators
  auto total = Add(Add(sum0, sum1), Add(sum2, sum3));

  // Process remaining full SIMD blocks
  for (; i + simd_lanes <= n; i += simd_lanes) {
    auto va = Load(tag, a + i);
    auto vb = Load(tag, b + i);
    total = MulAdd(va, vb, total);
  }

  // Process any final partial SIMD block
  if (i < n) {
    const auto mask = MaskFirstN(tag, n - i);
    auto va = LoadMask(tag, mask, a + i);
    auto vb = LoadMask(tag, mask, b + i);
    total = MulAdd(va, vb, total);
  }

  // Sum all SIMD lanes to get final scalar result
  return ReduceSum(tag, total);
}