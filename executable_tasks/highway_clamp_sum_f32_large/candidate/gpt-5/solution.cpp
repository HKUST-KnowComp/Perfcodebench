#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  if (n == 0) return 0.0f;

  hn::ScalableTag<float> d;
  const std::size_t L = hn::Lanes(d);

  auto zero = hn::Zero(d);
  auto s0 = hn::Zero(d);
  auto s1 = hn::Zero(d);
  auto s2 = hn::Zero(d);
  auto s3 = hn::Zero(d);

  std::size_t i = 0;

  // Process 4 vectors per iteration for ILP
  const std::size_t vec4 = (n / (4 * L));
  for (std::size_t blk = 0; blk < vec4; ++blk) {
    auto v0 = hn::LoadU(d, data + i + 0 * L);
    auto v1 = hn::LoadU(d, data + i + 1 * L);
    auto v2 = hn::LoadU(d, data + i + 2 * L);
    auto v3 = hn::LoadU(d, data + i + 3 * L);

    v0 = hn::Max(v0, zero);
    v1 = hn::Max(v1, zero);
    v2 = hn::Max(v2, zero);
    v3 = hn::Max(v3, zero);

    s0 = hn::Add(s0, v0);
    s1 = hn::Add(s1, v1);
    s2 = hn::Add(s2, v2);
    s3 = hn::Add(s3, v3);

    i += 4 * L;
  }

  auto vsum = hn::Add(hn::Add(s0, s1), hn::Add(s2, s3));

  // Process remaining full vectors
  const std::size_t vec_rem = (n - i) / L;
  for (std::size_t r = 0; r < vec_rem; ++r) {
    auto v = hn::LoadU(d, data + i);
    v = hn::Max(v, zero);
    vsum = hn::Add(vsum, v);
    i += L;
  }

  float sum = hn::SumOfLanes(d, vsum);

  // Scalar tail
  for (; i < n; ++i) {
    float x = data[i];
    if (x > 0.0f) sum += x;
  }

  return sum;
}
