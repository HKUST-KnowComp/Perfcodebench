#include "interface.h"
#include <hwy/highway.h>
#include <hwy/foreach_target.h>

HWY_BEFORE_NAMESPACE();
namespace hwy_clamp {
namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

float ClampSum(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const auto zero = hn::Zero(d);
  auto sum = hn::Zero(d);
  
  std::size_t i = 0;
  const std::size_t lanes = hn::Lanes(d);
  for (; i + lanes <= n; i += lanes) {
    auto v = hn::Load(d, data + i);
    v = hn::Max(v, zero);
    sum = hn::Add(sum, v);
  }
  
  float result = hn::ReduceSum(d, sum);
  
  for (; i < n; ++i) {
    float val = data[i];
    if (val > 0.0f) result += val;
  }
  
  return result;
}

} // namespace HWY_NAMESPACE
} // namespace hwy_clamp
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy_clamp {
HWY_EXPORT(ClampSum);
}

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(hwy_clamp::ClampSum)(data, n);
}
#endif