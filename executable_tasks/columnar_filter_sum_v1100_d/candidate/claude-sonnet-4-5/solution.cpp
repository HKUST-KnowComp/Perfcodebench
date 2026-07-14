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
    
    // Process in blocks to improve cache locality and enable better prefetching
    const std::size_t block_size = 256;
    std::size_t i = 0;
    
    // Main blocked loop
    for (; i + block_size <= n; i += block_size) {
      uint64_t block_sum = 0;
      
      // Inner loop optimized for auto-vectorization
      for (std::size_t j = 0; j < block_size; ++j) {
        const std::size_t idx = i + j;
        const uint32_t key = keys[idx];
        const uint32_t value = values[idx];
        const uint8_t flag = flags[idx];
        
        // Branchless predicate evaluation
        const bool passes = flag && (key < key_threshold) && ((key & 3u) == 0u);
        
        // Conditional accumulation using multiplication by boolean
        block_sum += static_cast<uint64_t>(value) * passes;
      }
      
      sum += block_sum;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      const uint32_t key = keys[i];
      const uint32_t value = values[i];
      const uint8_t flag = flags[i];
      
      const bool passes = flag && (key < key_threshold) && ((key & 3u) == 0u);
      sum += static_cast<uint64_t>(value) * passes;
    }
  }
  
  return sum;
}