#include "interface.h"
#include "hwy/highway.h"

float dot_product(const float* a, const float* b, std::size_t n) {
  float sum = 0.0f;
  hwy::HWY_FULL(float) d;
  auto zero = hwy::Zero(d);
  auto sum_vec = zero;
  const auto* HWY_RESTRICT pa = hwy::AlignedArray<float>(a);
  const auto* HWY_RESTRICT pb = hwy::AlignedArray<float>(b);
  const size_t n_vec = hwy::Lanes(d);
  size_t i = 0;
  for (; i + n_vec <= n; i += n_vec) {
    auto va = hwy::LoadU(d, pa + i);
    auto vb = hwy::LoadU(d, pb + i);
    sum_vec = hwy::MulAdd(va, vb, sum_vec);
  }
  sum = hwy::ReduceSum(d, sum_vec);
  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }
  return sum;
}