#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in blocks of 4 for better instruction-level parallelism
    std::size_t i = 0;
    const std::size_t block_end = n - (n % 4);
    
    for (; i < block_end; i += 4) {
      // Prefetch next cache lines
      __builtin_prefetch(&keys[i + 64], 0, 0);
      __builtin_prefetch(&flags[i + 64], 0, 0);
      __builtin_prefetch(&payloads[i + 64], 0, 0);
      
      // Unroll 4 iterations
      uint32_t k0 = keys[i];
      uint32_t k1 = keys[i + 1];
      uint32_t k2 = keys[i + 2];
      uint32_t k3 = keys[i + 3];
      
      uint8_t f0 = flags[i];
      uint8_t f1 = flags[i + 1];
      uint8_t f2 = flags[i + 2];
      uint8_t f3 = flags[i + 3];
      
      // Branchless predicate evaluation using bitwise operations
      // Convert boolean conditions to 0 or 1, then multiply
      uint32_t mask0 = (f0 != 0) & (k0 < threshold) & ((k0 & 7u) == 0u);
      uint32_t mask1 = (f1 != 0) & (k1 < threshold) & ((k1 & 7u) == 0u);
      uint32_t mask2 = (f2 != 0) & (k2 < threshold) & ((k2 & 7u) == 0u);
      uint32_t mask3 = (f3 != 0) & (k3 < threshold) & ((k3 & 7u) == 0u);
      
      // Conditional accumulation using mask (0 or 0xFFFFFFFF)
      mask0 = -mask0;
      mask1 = -mask1;
      mask2 = -mask2;
      mask3 = -mask3;
      
      sum += (static_cast<uint64_t>(payloads[i]) & mask0);
      sum += (static_cast<uint64_t>(payloads[i + 1]) & mask1);
      sum += (static_cast<uint64_t>(payloads[i + 2]) & mask2);
      sum += (static_cast<uint64_t>(payloads[i + 3]) & mask3);
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = keys[i];
      uint8_t f = flags[i];
      uint32_t mask = (f != 0) & (k < threshold) & ((k & 7u) == 0u);
      mask = -mask;
      sum += (static_cast<uint64_t>(payloads[i]) & mask);
    }
  }
  
  return sum;
}