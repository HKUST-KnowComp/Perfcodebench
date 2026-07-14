#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy_impl {
namespace HWY_NAMESPACE {

float DotProduct(const float* a, const float* b, std::size_t n) {
  const hwy::ScalableTag<float> d;
  auto sum = Zero(d);
  std::size_t i = 0;
  const std::size_t lanes = Lanes(d);
  for (; i + lanes <= n; i += lanes) {
    sum = MulAdd(Load(d, a + i), Load(d, b + i), sum);
  }
  float result = ReduceSum(d, sum);
  for (; i < n; ++i) {
    result += a[i] * b[i];
  }
  return result;
}

} // namespace HWY_NAMESPACE
} // namespace hwy_impl
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy_impl {
HWY_EXPORT(DotProduct);
} // namespace hwy_impl

float dot_product(const float* a, const float* b, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(hwy_impl::DotProduct)(a, b, n);
}
#endif