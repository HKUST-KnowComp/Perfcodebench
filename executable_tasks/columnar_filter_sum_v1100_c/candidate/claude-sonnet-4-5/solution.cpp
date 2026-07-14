#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in chunks of 4 for better ILP and potential SIMD
    std::size_t i = 0;
    const std::size_t n_unroll = n - (n % 4);
    
    for (; i < n_unroll; i += 4) {
      // Unroll 4 iterations
      uint32_t k0 = keys[i];
      uint32_t k1 = keys[i+1];
      uint32_t k2 = keys[i+2];
      uint32_t k3 = keys[i+3];
      
      // Compute predicates
      bool p0 = flags[i] && k0 < key_threshold && ((k0 & 3u) == 0u);
      bool p1 = flags[i+1] && k1 < key_threshold && ((k1 & 3u) == 0u);
      bool p2 = flags[i+2] && k2 < key_threshold && ((k2 & 3u) == 0u);
      bool p3 = flags[i+3] && k3 < key_threshold && ((k3 & 3u) == 0u);
      
      // Branchless accumulation using conditional move semantics
      sum += p0 ? static_cast<uint64_t>(values[i]) : 0;
      sum += p1 ? static_cast<uint64_t>(values[i+1]) : 0;
      sum += p2 ? static_cast<uint64_t>(values[i+2]) : 0;
      sum += p3 ? static_cast<uint64_t>(values[i+3]) : 0;
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      uint32_t k = keys[i];
      if (flags[i] && k < key_threshold && ((k & 3u) == 0u)) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  
  return sum;
}