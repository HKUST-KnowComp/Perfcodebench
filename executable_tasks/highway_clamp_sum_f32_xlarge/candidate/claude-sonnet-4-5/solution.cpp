#include "interface.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  const hn::ScalableTag<float> d;
  const auto zero = hn::Zero(d);
  const size_t N = hn::Lanes(d);
  
  // Use 4 accumulators to hide latency
  auto sum0 = zero;
  auto sum1 = zero;
  auto sum2 = zero;
  auto sum3 = zero;
  
  size_t i = 0;
  
  // Process 4 vectors at a time
  const size_t unroll = N * 4;
  for (; i + unroll <= n; i += unroll) {
    auto v0 = hn::LoadU(d, data + i);
    auto v1 = hn::LoadU(d, data + i + N);
    auto v2 = hn::LoadU(d, data + i + N * 2);
    auto v3 = hn::LoadU(d, data + i + N * 3);
    
    sum0 = hn::Add(sum0, hn::Max(v0, zero));
    sum1 = hn::Add(sum1, hn::Max(v1, zero));
    sum2 = hn::Add(sum2, hn::Max(v2, zero));
    sum3 = hn::Add(sum3, hn::Max(v3, zero));
  }
  
  // Process remaining full vectors
  for (; i + N <= n; i += N) {
    auto v = hn::LoadU(d, data + i);
    sum0 = hn::Add(sum0, hn::Max(v, zero));
  }
  
  // Combine accumulators
  sum0 = hn::Add(hn::Add(sum0, sum1), hn::Add(sum2, sum3));
  
  // Handle remainder with masked load
  if (i < n) {
    const size_t remaining = n - i;
    auto mask = hn::FirstN(d, remaining);
    auto v = hn::MaskedLoad(mask, d, data + i);
    sum0 = hn::Add(sum0, hn::Max(v, zero));
  }
  
  return hn::ReduceSum(d, sum0);
}
