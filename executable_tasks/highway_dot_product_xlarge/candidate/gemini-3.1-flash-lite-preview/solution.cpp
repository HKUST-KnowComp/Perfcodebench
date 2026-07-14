#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float dot_product(const float* a, const float* b, std::size_t n) {
  using D = ScalableTag<float>;
  const D d;
  const size_t lanes = Lanes(d);
  Vec<D> sum_vec = Zero(d);

  size_t i = 0;
  for (; i + lanes <= n; i += lanes) {
    const auto va = Load(d, a + i);
    const auto vb = Load(d, b + i);
    sum_vec = MulAdd(va, vb, sum_vec);
  }

  float sum = ReduceSum(d, sum_vec);

  if (i < n) {
    const auto mask = FirstN(d, n - i);
    const auto va = MaskedLoad(Zero(d), mask, a + i);
    const auto vb = MaskedLoad(Zero(d), mask, b + i);
    sum += ReduceSum(d, Mul(va, vb));
  }

  return sum;
}

} // namespace HWY_NAMESPACE
} // namespace hwy
HWY_AFTER_NAMESPACE();

float dot_product(const float* a, const float* b, std::size_t n) {
  return hwy::HWY_NAMESPACE::dot_product(a, b, n);
}