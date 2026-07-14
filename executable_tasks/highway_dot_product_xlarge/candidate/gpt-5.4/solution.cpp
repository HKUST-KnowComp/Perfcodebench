#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace hn = hwy::HWY_NAMESPACE;

float dot_product_scalar_tail(const float* a, const float* b, std::size_t i,
                              std::size_t n, float sum) {
  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }
  return sum;
}
}  // namespace

float dot_product(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  const std::size_t lanes = hn::Lanes(d);

  std::size_t i = 0;
  auto vsum0 = hn::Zero(d);
  auto vsum1 = hn::Zero(d);
  auto vsum2 = hn::Zero(d);
  auto vsum3 = hn::Zero(d);

  const std::size_t unroll = 4 * lanes;
  for (; i + unroll <= n; i += unroll) {
    const auto a0 = hn::LoadU(d, a + i);
    const auto b0 = hn::LoadU(d, b + i);
    const auto a1 = hn::LoadU(d, a + i + lanes);
    const auto b1 = hn::LoadU(d, b + i + lanes);
    const auto a2 = hn::LoadU(d, a + i + 2 * lanes);
    const auto b2 = hn::LoadU(d, b + i + 2 * lanes);
    const auto a3 = hn::LoadU(d, a + i + 3 * lanes);
    const auto b3 = hn::LoadU(d, b + i + 3 * lanes);

    vsum0 = hn::MulAdd(a0, b0, vsum0);
    vsum1 = hn::MulAdd(a1, b1, vsum1);
    vsum2 = hn::MulAdd(a2, b2, vsum2);
    vsum3 = hn::MulAdd(a3, b3, vsum3);
  }

  auto vsum = hn::Add(hn::Add(vsum0, vsum1), hn::Add(vsum2, vsum3));

  for (; i + lanes <= n; i += lanes) {
    const auto va = hn::LoadU(d, a + i);
    const auto vb = hn::LoadU(d, b + i);
    vsum = hn::MulAdd(va, vb, vsum);
  }

  float sum = hn::GetLane(hn::SumOfLanes(d, vsum));
  return dot_product_scalar_tail(a, b, i, n, sum);
}
