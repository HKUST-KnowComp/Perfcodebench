#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

float dot_product_impl(const float* a, const float* b, std::size_t n) {
  const ScalableTag<float> d;
  const std::size_t N = Lanes(d);
  
  auto sum0 = Zero(d);
  auto sum1 = Zero(d);
  auto sum2 = Zero(d);
  auto sum3 = Zero(d);
  
  std::size_t i = 0;
  for (; i + 4 * N <= n; i += 4 * N) {
    sum0 = MulAdd(LoadU(d, a + i),      LoadU(d, b + i),      sum0);
    sum1 = MulAdd(LoadU(d, a + i + N),  LoadU(d, b + i + N),  sum1);
    sum2 = MulAdd(LoadU(d, a + i + 2*N),LoadU(d, b + i + 2*N),sum2);
    sum3 = MulAdd(LoadU(d, a + i + 3*N),LoadU(d, b + i + 3*N),sum3);
  }
  
  sum0 = Add(sum0, sum1);
  sum2 = Add(sum2, sum3);
  sum0 = Add(sum0, sum2);
  
  for (; i + N <= n; i += N) {
    sum0 = MulAdd(LoadU(d, a + i), LoadU(d, b + i), sum0);
  }
  
  float total = ReduceSum(d, sum0);
  
  for (; i < n; ++i) {
    total += a[i] * b[i];
  }
  return total;
}

} // namespace HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace HWY_NAMESPACE {
HWY_EXPORT(dot_product_impl);
}

float dot_product(const float* a, const float* b, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(dot_product_impl)(a, b, n);
}
#endif