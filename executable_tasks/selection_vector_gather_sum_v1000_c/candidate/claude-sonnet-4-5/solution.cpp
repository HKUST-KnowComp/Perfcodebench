#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in blocks of 4 for better ILP
    std::size_t i = 0;
    const std::size_t n_unroll = n - (n % 4);
    
    for (; i < n_unroll; i += 4) {
      // Unroll 4 iterations
      uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
      
      if (__builtin_expect(flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u), 0)) {
        s0 = payloads[i];
      }
      if (__builtin_expect(flags[i+1] && keys[i+1] < threshold && ((keys[i+1] & 7u) == 0u), 0)) {
        s1 = payloads[i+1];
      }
      if (__builtin_expect(flags[i+2] && keys[i+2] < threshold && ((keys[i+2] & 7u) == 0u), 0)) {
        s2 = payloads[i+2];
      }
      if (__builtin_expect(flags[i+3] && keys[i+3] < threshold && ((keys[i+3] & 7u) == 0u), 0)) {
        s3 = payloads[i+3];
      }
      
      sum += s0 + s1 + s2 + s3;
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      if (__builtin_expect(flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u), 0)) {
        sum += payloads[i];
      }
    }
  }
  
  return sum;
}