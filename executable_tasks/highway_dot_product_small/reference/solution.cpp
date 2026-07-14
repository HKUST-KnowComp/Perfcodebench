#include "interface.h"

#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hn = hwy::HWY_NAMESPACE;
namespace hwy {
namespace HWY_NAMESPACE {

HWY_ATTR float DotImpl(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  auto sum = hn::Zero(d);
  std::size_t i = 0;
  for (; i + hn::Lanes(d) <= n; i += hn::Lanes(d)) {
    sum = hn::MulAdd(hn::LoadU(d, a + i), hn::LoadU(d, b + i), sum);
  }
  float out = hn::ReduceSum(d, sum);
  for (; i < n; ++i) {
    out += a[i] * b[i];
  }
  return out;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy
HWY_AFTER_NAMESPACE();

float dot_product(const float* a, const float* b, std::size_t n) {
  return hwy::HWY_NAMESPACE::DotImpl(a, b, n);
}
