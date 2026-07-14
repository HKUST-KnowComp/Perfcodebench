#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float dot_product(const float* a, const float* b, std::size_t n) {
  const auto d = Float32();
  const size_t VL = Lanes(d);
  
  float sum = 0.0f;
  
  // Process in SIMD chunks
  size_t i = 0;
  for (; i + VL <= n; i += VL) {
    const auto va = Load(d, a + i);
    const auto vb = Load(d, b + i);
    const auto prod = Mul(va, vb);
    sum += ReduceSum(prod);
  }
  
  // Handle remaining elements
  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }
  
  return sum;
}

} // namespace HWY_NAMESPACE
} // namespace hwy
HWY_AFTER_NAMESPACE();

HWY_EXPORT(dot_product);

float dot_product(const float* a, const float* b, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(dot_product)(a, b, n);
}