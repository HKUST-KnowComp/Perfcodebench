#include "interface.h"
#include "hwy/highway.h"

float clamp_sum(const float* data, std::size_t n) {
  namespace hn = hwy::HWY_NAMESPACE;
  const hn::ScalableTag<float> d;
  const std::size_t lanes = hn::Lanes(d);

  std::size_t i = 0;
  auto vsum = hn::Zero(d);
  const auto vzero = hn::Zero(d);

  const std::size_t unroll = 4;
  const std::size_t step = unroll * lanes;
  const std::size_t limit = n & ~(step - 1);

  for (; i < limit; i += step) {
    const auto v0 = hn::Load(d, data + i);
    const auto v1 = hn::Load(d, data + i + lanes);
    const auto v2 = hn::Load(d, data + i + 2 * lanes);
    const auto v3 = hn::Load(d, data + i + 3 * lanes);

    vsum = hn::Add(vsum, hn::Max(v0, vzero));
    vsum = hn::Add(vsum, hn::Max(v1, vzero));
    vsum = hn::Add(vsum, hn::Max(v2, vzero));
    vsum = hn::Add(vsum, hn::Max(v3, vzero));
  }

  const std::size_t vec_limit = n & ~(lanes - 1);
  for (; i < vec_limit; i += lanes) {
    const auto v = hn::Load(d, data + i);
    vsum = hn::Add(vsum, hn::Max(v, vzero));
  }

  float sum = hn::GetLane(hn::SumOfLanes(d, vsum));
  for (; i < n; ++i) {
    const float x = data[i];
    sum += x > 0.0f ? x : 0.0f;
  }
  return sum;
}
