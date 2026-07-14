#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace highway_clamp_sum_f32_xlarge {
namespace HWY_NAMESPACE {

float ClampSum(const float* data, std::size_t n) {
  const hwy::ScalableTag<float> d;
  const auto zero = hwy::Zero(d);
  auto sum0 = hwy::Zero(d);
  auto sum1 = hwy::Zero(d);
  auto sum2 = hwy::Zero(d);
  auto sum3 = hwy::Zero(d);
  
  std::size_t i = 0;
  const std::size_t lanes = hwy::Lanes(d);
  const std::size_t limit = n - (n % (lanes * 4));
  
  for (; i < limit; i += lanes * 4) {
    auto v0 = hwy::LoadU(d, data + i);
    auto v1 = hwy::LoadU(d, data + i + lanes);
    auto v2 = hwy::LoadU(d, data + i + lanes * 2);
    auto v3 = hwy::LoadU(d, data + i + lanes * 3);
    
    v0 = hwy::Max(v0, zero);
    v1 = hwy::Max(v1, zero);
    v2 = hwy::Max(v2, zero);
    v3 = hwy::Max(v3, zero);
    
    sum0 += v0;
    sum1 += v1;
    sum2 += v2;
    sum3 += v3;
  }
  
  sum0 += sum1;
  sum0 += sum2;
  sum0 += sum3;
  
  for (; i + lanes <= n; i += lanes) {
    auto v = hwy::LoadU(d, data + i);
    v = hwy::Max(v, zero);
    sum0 += v;
  }
  
  float sum = hwy::ReduceSum(d, sum0);
  
  for (; i < n; ++i) {
    float val = data[i];
    sum += (val > 0.0f) ? val : 0.0f;
  }
  return sum;
}

} // namespace HWY_NAMESPACE
} // namespace highway_clamp_sum_f32_xlarge
HWY_AFTER_NAMESPACE();

float clamp_sum(const float* data, std::size_t n) {
  return HWY_DYNAMIC_DISPATCH(highway_clamp_sum_f32_xlarge::ClampSum)(data, n);
}
