#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const auto zero = hn::Zero(d);
  
  // Kahan summation for numerical stability
  auto vec_sum = hn::Zero(d);
  auto vec_c = hn::Zero(d);
  
  const std::size_t lanes = hn::Lanes(d);
  std::size_t i = 0;
  
  // Process full vectors
  for (; i + lanes <= n; i += lanes) {
    auto v = hn::LoadU(d, data + i);
    auto clamped = hn::Max(v, zero);
    
    // Kahan summation
    auto y = hn::Sub(clamped, vec_c);
    auto t = hn::Add(vec_sum, y);
    vec_c = hn::Sub(hn::Sub(t, vec_sum), y);
    vec_sum = t;
  }
  
  // Horizontal reduction with Kahan summation
  float sum = 0.0f;
  float c = 0.0f;
  
  alignas(64) float sum_buf[hn::MaxLanes(d)];
  alignas(64) float c_buf[hn::MaxLanes(d)];
  hn::StoreU(vec_sum, d, sum_buf);
  hn::StoreU(vec_c, d, c_buf);
  
  for (std::size_t j = 0; j < lanes; ++j) {
    float y = sum_buf[j] - c;
    float t = sum + y;
    c = (t - sum) - y;
    sum = t;
  }
  
  // Handle tail with Kahan summation
  for (; i < n; ++i) {
    float val = data[i] > 0.0f ? data[i] : 0.0f;
    float y = val - c;
    float t = sum + y;
    c = (t - sum) - y;
    sum = t;
  }
  
  return sum;
}