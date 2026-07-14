#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace hw = hwy::HWY_NAMESPACE;

template <typename T> 
float DotProductLoop(const T* a, const T* b, std::size_t n) {
  const HWY::Simd<float> d = hw::Load("v");
  const size_t lanes = hw::Lanes(d);
  hw::Simd<float> sum_vec = hw::SetZero("v");

  size_t i = 0;
  for (; i + lanes <= n; i += lanes) {
    hw::Simd<float> va = hw::Load("v", a + i);
    hw::Simd<float> vb = hw::Load("v", b + i);
    sum_vec = hw::Add("v", sum_vec, hw::Mul("v", va, vb));
  }

  float sum = hw::ReduceSum("v", sum_vec);

  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }

  return sum;
}

// Dispatcher to select the best SIMD implementation at runtime
float DotProductDispatcher(const float* a, const float* b, std::size_t n) {
  return HWY_DISPATCH(DotProductLoop<float>)(a, b, n);
}
}

float dot_product(const float* a, const float* b, std::size_t n) {
  if (n == 0) return 0.0f;
  return DotProductDispatcher(a, b, n);
}