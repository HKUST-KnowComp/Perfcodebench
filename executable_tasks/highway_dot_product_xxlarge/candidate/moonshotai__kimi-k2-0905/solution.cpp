#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float dot_product(const float* HWY_RESTRICT a,
                  const float* HWY_RESTRICT b,
                  std::size_t n) {
  const hn::ScalableTag<float> d;
  const std::size_t kLanes = hn::Lanes(d);

  hn::Vec<decltype(d)> sum0 = hn::Zero(d);
  hn::Vec<decltype(d)> sum1 = hn::Zero(d);
  hn::Vec<decltype(d)> sum2 = hn::Zero(d);
  hn::Vec<decltype(d)> sum3 = hn::Zero(d);

  std::size_t i = 0;
  for (; i + 4 * kLanes <= n; i += 4 * kLanes) {
    sum0 = hn::MulAdd(hn::Load(d, a + i + 0 * kLanes),
                      hn::Load(d, b + i + 0 * kLanes), sum0);
    sum1 = hn::MulAdd(hn::Load(d, a + i + 1 * kLanes),
                      hn::Load(d, b + i + 1 * kLanes), sum1);
    sum2 = hn::MulAdd(hn::Load(d, a + i + 2 * kLanes),
                      hn::Load(d, b + i + 2 * kLanes), sum2);
    sum3 = hn::MulAdd(hn::Load(d, a + i + 3 * kLanes),
                      hn::Load(d, b + i + 3 * kLanes), sum3);
  }
  for (; i + kLanes <= n; i += kLanes) {
    sum0 = hn::MulAdd(hn::Load(d, a + i), hn::Load(d, b + i), sum0);
  }

  sum0 = hn::Add(sum0, hn::Add(sum1, hn::Add(sum2, sum3)));
  float scalar_sum = hn::ReduceSum(d, sum0);

  for (; i < n; ++i) scalar_sum += a[i] * b[i];
  return scalar_sum;
}