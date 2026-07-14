#include "interface.h"
#include <hwy/highway.h>

namespace {
namespace HW = hwy::HW;

// Define the SIMD function template
template <typename T> 
float DotProductSIMD(const T* a, const T* b, std::size_t n) {
  const HW::ScalableTag<float> d; // Use scalable vectors for float
  auto sum_vec = HW::SetZero(d);
  
  std::size_t i = 0;
  // Process elements in chunks of the SIMD lane width
  for (; i + HW::Lanes(d) <= n; i += HW::Lanes(d)) {
    auto va = HW::Load(d, a + i);
    auto vb = HW::Load(d, b + i);
    sum_vec = HW::Add(sum_vec, HW::Mul(va, vb));
  }

  // Horizontal reduction of the SIMD vector
  float sum = HW::ReduceSum(d, sum_vec);

  // Scalar tail processing for remaining elements
  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }

  return sum;
}
}

float dot_product(const float* a, const float* b, std::size_t n) {
  if (n == 0) return 0.0f;
  // Highway handles dispatching to the best available instruction set at runtime
  return HW::Run<DotProductSIMD>(a, b, n);
}