#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float dot_product(const float* a, const float* b, std::size_t n) {
  using D = hn::ScalableTag<float>;
  D d;
  auto sum = hn::Zero(d);
  std::size_t i = 0;
  for (; i + hn::Lanes(d) <= n; i += hn::Lanes(d)) {
    auto va = hn::LoadU(d, a + i);
    auto vb = hn::LoadU(d, b + i);
    sum = hn::Add(sum, hn::Mul(va, vb));
  }
  float total = hn::ReduceSum(d, sum);
  for (; i < n; ++i) {
    total += a[i] * b[i];
  }
  return total;
}
