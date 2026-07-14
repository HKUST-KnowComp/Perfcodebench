#include "interface.h"
#include <hwy/highway.h>

namespace {

namespace hn = hwy::HWY_NAMESPACE;

}  // namespace

float dot_product(const float* a, const float* b, std::size_t n) {
  using D = hn::ScalableTag<float>;
  const D d;
  const std::size_t lanes = hn::Lanes(d);

  std::size_t i = 0;

  // Multiple independent accumulators improve throughput on large vectors by
  // reducing the dependency chain length.
  auto sum0 = hn::Zero(d);
  auto sum1 = hn::Zero(d);
  auto sum2 = hn::Zero(d);
  auto sum3 = hn::Zero(d);

  const std::size_t unroll = 4 * lanes;
  const std::size_t limit = n & ~(unroll - 1);

  for (; i < limit; i += unroll) {
    const auto a0 = hn::LoadU(d, a + i);
    const auto b0 = hn::LoadU(d, b + i);
    const auto a1 = hn::LoadU(d, a + i + lanes);
    const auto b1 = hn::LoadU(d, b + i + lanes);
    const auto a2 = hn::LoadU(d, a + i + 2 * lanes);
    const auto b2 = hn::LoadU(d, b + i + 2 * lanes);
    const auto a3 = hn::LoadU(d, a + i + 3 * lanes);
    const auto b3 = hn::LoadU(d, b + i + 3 * lanes);

    sum0 = hn::MulAdd(a0, b0, sum0);
    sum1 = hn::MulAdd(a1, b1, sum1);
    sum2 = hn::MulAdd(a2, b2, sum2);
    sum3 = hn::MulAdd(a3, b3, sum3);
  }

  auto sum = hn::Add(hn::Add(sum0, sum1), hn::Add(sum2, sum3));

  const std::size_t vec_limit = n & ~(lanes - 1);
  for (; i < vec_limit; i += lanes) {
    sum = hn::MulAdd(hn::LoadU(d, a + i), hn::LoadU(d, b + i), sum);
  }

  float result = hn::GetLane(hn::SumOfLanes(d, sum));

  for (; i < n; ++i) {
    result += a[i] * b[i];
  }

  return result;
}
