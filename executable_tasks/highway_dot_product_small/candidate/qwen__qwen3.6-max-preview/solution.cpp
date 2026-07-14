#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float dot_product(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  auto sum = hn::Zero(d);
  std::size_t i = 0;
  const std::size_t N = hn::Lanes(d);

  for (; i + N <= n; i += N) {
    sum = hn::MulAdd(hn::LoadU(d, a + i), hn::LoadU(d, b + i), sum);
  }

  float total = hn::GetLane(hn::SumOfLanes(d, sum));
  for (; i < n; ++i) {
    total += a[i] * b[i];
  }
  return total;
}
