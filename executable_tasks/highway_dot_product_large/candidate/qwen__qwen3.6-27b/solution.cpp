#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace dot_product_ns {
HWY_NAMESPACE {

float dot_product_impl(const float* a, const float* b, std::size_t n) {
  HWY_FULL(float) d;
  const size_t N = d.Lanes();
  size_t i = 0;
  V sum = Zero(d);
  
  for (; i + N <= n; i += N) {
    V va = LoadU(d, a + i);
    V vb = LoadU(d, b + i);
    sum = Add(sum, Mul(va, vb));
  }
  
  float result = GetLane(ReduceSum(d, sum), 0);
  for (; i < n; ++i) {
    result += a[i] * b[i];
  }
  return result;
}

} // namespace
} // namespace dot_product_ns
HWY_AFTER_NAMESPACE();

float dot_product(const float* a, const float* b, std::size_t n) {
  return dot_product_ns::HWY_NAMESPACE::dot_product_impl(a, b, n);
}