#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

float ClampSumImpl(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const std::size_t N = hn::Lanes(d);
  const auto zero = hn::Zero(d);
  
  auto sum0 = zero;
  auto sum1 = zero;
  auto sum2 = zero;
  auto sum3 = zero;
  
  std::size_t i = 0;
  for (; i + 4 * N <= n; i += 4 * N) {
    auto v0 = hn::LoadU(d, data + i);
    auto v1 = hn::LoadU(d, data + i + N);
    auto v2 = hn::LoadU(d, data + i + 2 * N);
    auto v3 = hn::LoadU(d, data + i + 3 * N);
    
    sum0 = hn::Add(sum0, hn::Max(v0, zero));
    sum1 = hn::Add(sum1, hn::Max(v1, zero));
    sum2 = hn::Add(sum2, hn::Max(v2, zero));
    sum3 = hn::Add(sum3, hn::Max(v3, zero));
  }
  
  sum0 = hn::Add(hn::Add(sum0, sum1), hn::Add(sum2, sum3));
  
  for (; i + N <= n; i += N) {
    auto v = hn::LoadU(d, data + i);
    sum0 = hn::Add(sum0, hn::Max(v, zero));
  }
  
  float sum = hn::GetLane(hn::SumOfLanes(d, sum0));
  
  for (; i < n; ++i) {
    float val = data[i];
    sum += val > 0.0f ? val : 0.0f;
  }
  return sum;
}

}  // namespace HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace {
HWY_EXPORT(ClampSumImpl);
}

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(ClampSumImpl)(data, n);
}
#endif