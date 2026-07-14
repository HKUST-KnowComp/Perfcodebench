#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

float dot_product_impl(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  const size_t N = hn::Lanes(d);
  auto sum_vec = hn::Zero(d);
  size_t i = 0;
  for (; i + N <= n; i += N) {
    auto va = hn::LoadU(d, a + i);
    auto vb = hn::LoadU(d, b + i);
    sum_vec = hn::MulAdd(va, vb, sum_vec);
  }
  float sum = hn::ReduceSum(d, sum_vec);
  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }
  return sum;
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
