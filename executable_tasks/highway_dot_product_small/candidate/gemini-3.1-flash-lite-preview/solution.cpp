#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float dot_product_simd(const float* a, const float* b, std::size_t n) {
  using D = HWY_FULL(float);
  const D d;
  const size_t lanes = Lanes(d);
  auto sum_v = Zero(d);
  size_t i = 0;

  for (; i + lanes <= n; i += lanes) {
    const auto va = Load(d, a + i);
    const auto vb = Load(d, b + i);
    sum_v = MulAdd(va, vb, sum_v);
  }

  float sum = ReduceSum(d, sum_v);
  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }
  return sum;
}

} // namespace HWY_NAMESPACE
} // namespace hwy
HWY_AFTER_NAMESPACE();

float dot_product(const float* a, const float* b, std::size_t n) {
  return hwy::HWY_NAMESPACE::dot_product_simd(a, b, n);
}