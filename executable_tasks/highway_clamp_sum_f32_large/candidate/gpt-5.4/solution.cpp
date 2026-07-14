#include "interface.h"
#include "hwy/highway.h"

namespace {
namespace hn = hwy::HWY_NAMESPACE;
}

float clamp_sum(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const std::size_t lanes = hn::Lanes(d);

  std::size_t i = 0;
  auto zero = hn::Zero(d);
  auto sum0 = hn::Zero(d);
  auto sum1 = hn::Zero(d);
  auto sum2 = hn::Zero(d);
  auto sum3 = hn::Zero(d);

  const std::size_t step = lanes * 4;
  for (; i + step <= n; i += step) {
    auto v0 = hn::LoadU(d, data + i);
    auto v1 = hn::LoadU(d, data + i + lanes);
    auto v2 = hn::LoadU(d, data + i + 2 * lanes);
    auto v3 = hn::LoadU(d, data + i + 3 * lanes);

    sum0 = hn::Add(sum0, hn::Max(v0, zero));
    sum1 = hn::Add(sum1, hn::Max(v1, zero));
    sum2 = hn::Add(sum2, hn::Max(v2, zero));
    sum3 = hn::Add(sum3, hn::Max(v3, zero));
  }

  auto vsum = hn::Add(hn::Add(sum0, sum1), hn::Add(sum2, sum3));

  for (; i + lanes <= n; i += lanes) {
    auto v = hn::LoadU(d, data + i);
    vsum = hn::Add(vsum, hn::Max(v, zero));
  }

  float sum = hn::GetLane(hn::SumOfLanes(d, vsum));

  for (; i < n; ++i) {
    const float x = data[i];
    sum += x > 0.0f ? x : 0.0f;
  }

  return sum;
}
