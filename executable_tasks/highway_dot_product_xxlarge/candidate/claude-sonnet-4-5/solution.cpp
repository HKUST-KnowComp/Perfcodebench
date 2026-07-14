#include "interface.h"
#include <hwy/highway.h>

namespace hn = hwy::HWY_NAMESPACE;

float dot_product(const float* a, const float* b, std::size_t n) {
  const hn::ScalableTag<float> d;
  const size_t N = hn::Lanes(d);
  
  if (n == 0) return 0.0f;
  
  // Process 4 vectors at a time for better throughput
  const size_t n_unroll = (n / (N * 4)) * (N * 4);
  
  auto sum0 = hn::Zero(d);
  auto sum1 = hn::Zero(d);
  auto sum2 = hn::Zero(d);
  auto sum3 = hn::Zero(d);
  
  size_t i = 0;
  for (; i < n_unroll; i += N * 4) {
    auto va0 = hn::LoadU(d, a + i);
    auto vb0 = hn::LoadU(d, b + i);
    sum0 = hn::MulAdd(va0, vb0, sum0);
    
    auto va1 = hn::LoadU(d, a + i + N);
    auto vb1 = hn::LoadU(d, b + i + N);
    sum1 = hn::MulAdd(va1, vb1, sum1);
    
    auto va2 = hn::LoadU(d, a + i + N * 2);
    auto vb2 = hn::LoadU(d, b + i + N * 2);
    sum2 = hn::MulAdd(va2, vb2, sum2);
    
    auto va3 = hn::LoadU(d, a + i + N * 3);
    auto vb3 = hn::LoadU(d, b + i + N * 3);
    sum3 = hn::MulAdd(va3, vb3, sum3);
  }
  
  // Combine the 4 accumulators
  sum0 = hn::Add(sum0, sum1);
  sum2 = hn::Add(sum2, sum3);
  sum0 = hn::Add(sum0, sum2);
  
  // Process remaining full vectors
  for (; i + N <= n; i += N) {
    auto va = hn::LoadU(d, a + i);
    auto vb = hn::LoadU(d, b + i);
    sum0 = hn::MulAdd(va, vb, sum0);
  }
  
  // Horizontal reduction
  float result = hn::ReduceSum(d, sum0);
  
  // Handle tail elements with Kahan summation for accuracy
  float c = 0.0f;
  for (; i < n; ++i) {
    float prod = a[i] * b[i];
    float y = prod - c;
    float t = result + y;
    c = (t - result) - y;
    result = t;
  }
  
  return result;
}
