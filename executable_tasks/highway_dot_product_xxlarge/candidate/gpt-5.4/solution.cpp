#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace hn = hwy::HWY_NAMESPACE;

float DotProductHWY(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  using V = hn::Vec<decltype(d)>;

  std::size_t i = 0;
  V vsum = hn::Zero(d);

  const std::size_t lanes = hn::Lanes(d);
  const std::size_t unroll = 4;
  const std::size_t step = lanes * unroll;

  for (; i + step <= n; i += step) {
    vsum = hn::Add(vsum, hn::Mul(hn::LoadU(d, a + i), hn::LoadU(d, b + i)));
    vsum = hn::Add(vsum, hn::Mul(hn::LoadU(d, a + i + lanes), hn::LoadU(d, b + i + lanes)));
    vsum = hn::Add(vsum, hn::Mul(hn::LoadU(d, a + i + 2 * lanes), hn::LoadU(d, b + i + 2 * lanes)));
    vsum = hn::Add(vsum, hn::Mul(hn::LoadU(d, a + i + 3 * lanes), hn::LoadU(d, b + i + 3 * lanes)));
  }

  for (; i + lanes <= n; i += lanes) {
    vsum = hn::Add(vsum, hn::Mul(hn::LoadU(d, a + i), hn::LoadU(d, b + i)));
  }

  float sum = hn::GetLane(hn::SumOfLanes(d, vsum));

  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }

  return sum;
}
}  // namespace

float dot_product(const float* a, const float* b, std::size_t n) {
  return DotProductHWY(a, b, n);
}
