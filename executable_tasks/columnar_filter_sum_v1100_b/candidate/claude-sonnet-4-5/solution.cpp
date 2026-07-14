#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict__ k = keys.data();
  const uint32_t* __restrict__ v = values.data();
  const uint8_t* __restrict__ f = flags.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    
    // Unrolled loop for better ILP
    for (; i + 3 < n; i += 4) {
      uint32_t k0 = k[i];
      uint32_t k1 = k[i+1];
      uint32_t k2 = k[i+2];
      uint32_t k3 = k[i+3];
      
      bool cond0 = f[i] && (k0 < key_threshold) && ((k0 & 3u) == 0u);
      bool cond1 = f[i+1] && (k1 < key_threshold) && ((k1 & 3u) == 0u);
      bool cond2 = f[i+2] && (k2 < key_threshold) && ((k2 & 3u) == 0u);
      bool cond3 = f[i+3] && (k3 < key_threshold) && ((k3 & 3u) == 0u);
      
      s0 += cond0 ? static_cast<uint64_t>(v[i]) : 0;
      s1 += cond1 ? static_cast<uint64_t>(v[i+1]) : 0;
      s2 += cond2 ? static_cast<uint64_t>(v[i+2]) : 0;
      s3 += cond3 ? static_cast<uint64_t>(v[i+3]) : 0;
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      uint32_t ki = k[i];
      bool cond = f[i] && (ki < key_threshold) && ((ki & 3u) == 0u);
      s0 += cond ? static_cast<uint64_t>(v[i]) : 0;
    }
    
    sum = s0 + s1 + s2 + s3;
  }
  
  return sum;
}