#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

float DotProductHWY(const float* HWY_RESTRICT a, const float* HWY_RESTRICT b, std::size_t n) {
  const hwy::HWY_FULL(float) d;
  using V = hwy::Vec<decltype(d)>;

  V acc0 = hwy::Zero(d);
  V acc1 = hwy::Zero(d);
  V acc2 = hwy::Zero(d);
  V acc3 = hwy::Zero(d);

  const std::size_t L = hwy::Lanes(d);
  std::size_t i = 0;
  const std::size_t step = 4 * L;
  const std::size_t n4 = step ? (n / step) * step : 0;

  for (; i < n4; i += step) {
    acc0 = hwy::MulAdd(hwy::LoadU(d, a + i + 0 * L), hwy::LoadU(d, b + i + 0 * L), acc0);
    acc1 = hwy::MulAdd(hwy::LoadU(d, a + i + 1 * L), hwy::LoadU(d, b + i + 1 * L), acc1);
    acc2 = hwy::MulAdd(hwy::LoadU(d, a + i + 2 * L), hwy::LoadU(d, b + i + 2 * L), acc2);
    acc3 = hwy::MulAdd(hwy::LoadU(d, a + i + 3 * L), hwy::LoadU(d, b + i + 3 * L), acc3);
  }

  V acc = acc0 + acc1 + acc2 + acc3;

  for (; i + L <= n; i += L) {
    acc = hwy::MulAdd(hwy::LoadU(d, a + i), hwy::LoadU(d, b + i), acc);
  }

  float sum = 0.0f;
  alignas(64) float tmp[64];
  hwy::StoreU(acc, d, tmp);
  for (std::size_t j = 0; j < L; ++j) sum += tmp[j];

  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }

  return sum;
}

}  // namespace HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

float dot_product(const float* a, const float* b, std::size_t n) {
  return hwy::HWY_DYNAMIC_DISPATCH(DotProductHWY)(a, b, n);
}
