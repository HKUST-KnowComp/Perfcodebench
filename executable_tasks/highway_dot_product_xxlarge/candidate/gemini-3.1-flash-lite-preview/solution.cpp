#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

float dot_product(const float* a, const float* b, std::size_t n) {
  using namespace hwy::HWY_NAMESPACE;
  const ScalableTag<float> d;
  const size_t lanes = Lanes(d);
  
  // Use multiple accumulators to break dependency chains and improve ILP
  auto v0 = Zero(d);
  auto v1 = Zero(d);
  auto v2 = Zero(d);
  auto v3 = Zero(d);

  size_t i = 0;
  const size_t step = lanes * 4;
  for (; i + step <= n; i += step) {
    v0 = MulAdd(Load(d, a + i), Load(d, b + i), v0);
    v1 = MulAdd(Load(d, a + i + lanes), Load(d, b + i + lanes), v1);
    v2 = MulAdd(Load(d, a + i + 2 * lanes), Load(d, b + i + 2 * lanes), v2);
    v3 = MulAdd(Load(d, a + i + 3 * lanes), Load(d, b + i + 3 * lanes), v3);
  }

  // Combine accumulators
  auto sum_v = Add(Add(v0, v1), Add(v2, v3));

  // Handle remaining elements
  for (; i + lanes <= n; i += lanes) {
    sum_v = MulAdd(Load(d, a + i), Load(d, b + i), sum_v);
  }

  float total = ReduceSum(d, sum_v);

  // Tail cleanup
  for (; i < n; ++i) {
    total += a[i] * b[i];
  }

  return total;
}

} // namespace HWY_NAMESPACE
} // namespace hwy
HWY_AFTER_NAMESPACE();

float dot_product(const float* a, const float* b, std::size_t n) {
  return hwy::HWY_NAMESPACE::dot_product(a, b, n);
}