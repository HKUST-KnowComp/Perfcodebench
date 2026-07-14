#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace HWY =
    hwy::HWY_NAMESPACE;
namespace hn = hwy::HWY_NAMESPACE::HWY;
}

float dot_product(const float* a, const float* b, std::size_t n) {
  if (n == 0) return 0.0f;

  const HWY::ScalableTag<float> d;
  const size_t lanes = HWY::Lanes(d);
  const size_t step = lanes * sizeof(float);
  
  HWY::Simd<float> sum_vec = HWY::SetZero(d);
  std::size_t i = 0;

  // Main SIMD loop
  for (; i + lanes <= n; i += lanes) {
    HWY::Simd<float> va = HWY::Load(d, a + i);
    HWY::Simd<float> vb = HWY::Load(d, b + i);
    sum_vec = HWY::Add(sum_vec, HWY::Mul(va, vb));
  }

  // Horizontal reduction of the SIMD register
  HWY::Simd<float> abs_sum = HWY::ReduceSum(d, sum_vec);
  float total_sum = HWY::Get(abs_sum, 0);

  // Scalar tail loop for remaining elements
  for (; i < n; ++i) {
    total_sum += a[i] * b[i];
  }

  return total_sum;
}