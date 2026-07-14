#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float dot_product(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  const std::size_t N = hn::Lanes(d);
  
  auto sum0 = hn::Zero(d);
  auto sum1 = hn::Zero(d);
  auto sum2 = hn::Zero(d);
  auto sum3 = hn::Zero(d);
  
  std::size_t i = 0;
  for (; i + 4 * N <= n; i += 4 * N) {
    sum0 = hn::MulAdd(hn::LoadU(d, a + i), hn::LoadU(d, b + i), sum0);
    sum1 = hn::MulAdd(hn::LoadU(d, a + i + N), hn::LoadU(d, b + i + N), sum1);
    sum2 = hn::MulAdd(hn::LoadU(d, a + i + 2*N), hn::LoadU(d, b + i + 2*N), sum2);
    sum3 = hn::MulAdd(hn::LoadU(d, a + i + 3*N), hn::LoadU(d, b + i + 3*N), sum3);
  }
  
  sum0 = hn::Add(sum0, sum1);
  sum2 = hn::Add(sum2, sum3);
  sum0 = hn::Add(sum0, sum2);
  
  for (; i + N <= n; i += N) {
    sum0 = hn::MulAdd(hn::LoadU(d, a + i), hn::LoadU(d, b + i), sum0);
  }
  
  float res = hn::ReduceSum(d, sum0);
  
  for (; i < n; ++i) {
    res += a[i] * b[i];
  }
  return res;
}