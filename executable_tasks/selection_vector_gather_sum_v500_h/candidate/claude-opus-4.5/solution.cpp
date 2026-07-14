#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict k = keys.data();
  const uint8_t* __restrict f = flags.data();
  const uint32_t* __restrict p = payloads.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    uint64_t s4 = 0, s5 = 0, s6 = 0, s7 = 0;
    
    std::size_t i = 0;
    const std::size_t limit = n & ~std::size_t(7);
    
    for (; i < limit; i += 8) {
      uint32_t k0 = k[i], k1 = k[i+1], k2 = k[i+2], k3 = k[i+3];
      uint32_t k4 = k[i+4], k5 = k[i+5], k6 = k[i+6], k7 = k[i+7];
      
      uint32_t m0 = (f[i] != 0) & (k0 < threshold) & ((k0 & 7u) == 0u);
      uint32_t m1 = (f[i+1] != 0) & (k1 < threshold) & ((k1 & 7u) == 0u);
      uint32_t m2 = (f[i+2] != 0) & (k2 < threshold) & ((k2 & 7u) == 0u);
      uint32_t m3 = (f[i+3] != 0) & (k3 < threshold) & ((k3 & 7u) == 0u);
      uint32_t m4 = (f[i+4] != 0) & (k4 < threshold) & ((k4 & 7u) == 0u);
      uint32_t m5 = (f[i+5] != 0) & (k5 < threshold) & ((k5 & 7u) == 0u);
      uint32_t m6 = (f[i+6] != 0) & (k6 < threshold) & ((k6 & 7u) == 0u);
      uint32_t m7 = (f[i+7] != 0) & (k7 < threshold) & ((k7 & 7u) == 0u);
      
      s0 += static_cast<uint64_t>(p[i]) * m0;
      s1 += static_cast<uint64_t>(p[i+1]) * m1;
      s2 += static_cast<uint64_t>(p[i+2]) * m2;
      s3 += static_cast<uint64_t>(p[i+3]) * m3;
      s4 += static_cast<uint64_t>(p[i+4]) * m4;
      s5 += static_cast<uint64_t>(p[i+5]) * m5;
      s6 += static_cast<uint64_t>(p[i+6]) * m6;
      s7 += static_cast<uint64_t>(p[i+7]) * m7;
    }
    
    sum = s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
    
    for (; i < n; ++i) {
      uint32_t key = k[i];
      uint32_t mask = (f[i] != 0) & (key < threshold) & ((key & 7u) == 0u);
      sum += static_cast<uint64_t>(p[i]) * mask;
    }
  }
  
  return sum;
}