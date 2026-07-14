#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float dot_product(const float* a, const float* b, std::size_t n) {
  using D = HWY_FULL(float);
  const D d;
  const size_t lanes = Lanes(d);
  
  // Use multiple accumulators to reduce dependency chains
  auto v_sum0 = Zero(d);
  auto v_sum1 = Zero(d);
  auto v_sum2 = Zero(d);
  auto v_sum3 = Zero(d);

  size_t i = 0;
  const size_t step = lanes * 4;
  for (; i + step <= n; i += step) {
    v_sum0 = MulAdd(Load(d, a + i), Load(d, b + i), v_sum0);
    v_sum1 = MulAdd(Load(d, a + i + lanes), Load(d, b + i + lanes), v_sum1);
    v_sum2 = MulAdd(Load(d, a + i + 2 * lanes), Load(d, b + i + 2 * lanes), v_sum2);
    v_sum3 = MulAdd(Load(d, a + i + 3 * lanes), Load(d, b + i + 3 * lanes), v_sum3);
  }

  auto v_sum = v_sum0 + v_sum1 + v_sum2 + v_sum3;

  // Handle remaining elements
  for (; i + lanes <= n; i += lanes) {
    v_sum = MulAdd(Load(d, a + i), Load(d, b + i), v_sum);
  }

  float sum = ReduceSum(d, v_sum);

  if (i < n) {
    const size_t remaining = n - i;
    const auto mask = FirstN(d, remaining);
    const auto va = MaskedLoad(Zero(d), mask, a + i);
    const auto vb = MaskedLoad(Zero(d), mask, b + i);
    sum += ReduceSum(d, va * vb);
  }

  return sum;
}

} // namespace HWY_NAMESPACE
} // namespace hwy
HWY_AFTER_NAMESPACE();

float dot_product(const float* a, const float* b, std::size_t n) {
  return hwy::HWY_NAMESPACE::dot_product(a, b, n);
}