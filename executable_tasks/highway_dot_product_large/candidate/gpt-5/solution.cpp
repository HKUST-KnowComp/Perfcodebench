#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hwy {
namespace HWY_NAMESPACE {

static float DotInner(const float* a, const float* b, std::size_t n) {
  const ScalableTag<float> d;
  const std::size_t L = Lanes(d);

  std::size_t i = 0;
  // Unroll to improve ILP
  auto acc0 = Zero(d);
  auto acc1 = Zero(d);
  auto acc2 = Zero(d);
  auto acc3 = Zero(d);

  const std::size_t kUnroll = 4;
  const std::size_t vec_chunk = kUnroll * L;

  for (; i + vec_chunk <= n; i += vec_chunk) {
    const auto a0 = Load(d, a + i);
    const auto b0 = Load(d, b + i);
    acc0 = MulAdd(a0, b0, acc0);

    const auto a1 = Load(d, a + i + L);
    const auto b1 = Load(d, b + i + L);
    acc1 = MulAdd(a1, b1, acc1);

    const auto a2 = Load(d, a + i + 2 * L);
    const auto b2 = Load(d, b + i + 2 * L);
    acc2 = MulAdd(a2, b2, acc2);

    const auto a3 = Load(d, a + i + 3 * L);
    const auto b3 = Load(d, b + i + 3 * L);
    acc3 = MulAdd(a3, b3, acc3);
  }

  auto acc = Add(Add(acc0, acc1), Add(acc2, acc3));

  for (; i + L <= n; i += L) {
    const auto va = Load(d, a + i);
    const auto vb = Load(d, b + i);
    acc = MulAdd(va, vb, acc);
  }

  float sum = ReduceSum(d, acc);

  for (; i < n; ++i) {
    sum += a[i] * b[i];
  }

  return sum;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy {
HWY_EXPORT(DotInner);
}  // namespace hwy

float dot_product(const float* a, const float* b, std::size_t n) {
  return hwy::HWY_DYNAMIC_DISPATCH(DotInner)(a, b, n);
}
#endif
