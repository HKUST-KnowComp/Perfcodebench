#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy { namespace HWY_NAMESPACE {

float clamp_sum_impl(const float* data, size_t n) {
  using D = HWY_FULL(float);
  const D d;
  using V = decltype(Zero(d));
  const V vzero = Zero(d);
  V sumv = Zero(d);
  size_t i = 0;
  for (; i + Lanes(d) <= n; i += Lanes(d)) {
    V v = LoadU(d, data + i);
    V mask = Gt(v, vzero);
    V clamped = IfThenElseZero(mask, v);
    sumv = Add(sumv, clamped);
  }
  float sum = GetLane(SumOfLanes(d, sumv));
  // scalar remainder
  for (; i < n; ++i) {
    if (data[i] > 0.0f) sum += data[i];
  }
  return sum;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy { extern float (*HWY_EXPORT(clamp_sum_impl))(const float*, size_t); }
float clamp_sum(const float* data, size_t n) {
  return hwy::HWY_DYNAMIC_DISPATCH(clamp_sum_impl)(data, n);
}
#endif