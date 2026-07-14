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
    
    // Process in blocks of 4 for better ILP and potential vectorization
    std::size_t i = 0;
    const std::size_t block_end = n - (n % 4);
    
    for (; i < block_end; i += 4) {
      // Unroll 4 iterations
      uint32_t k0 = keys[i];
      uint32_t k1 = keys[i + 1];
      uint32_t k2 = keys[i + 2];
      uint32_t k3 = keys[i + 3];
      
      uint8_t f0 = flags[i];
      uint8_t f1 = flags[i + 1];
      uint8_t f2 = flags[i + 2];
      uint8_t f3 = flags[i + 3];
      
      // Branchless predicate evaluation
      bool pred0 = f0 && (k0 < threshold) && ((k0 & 7u) == 0u);
      bool pred1 = f1 && (k1 < threshold) && ((k1 & 7u) == 0u);
      bool pred2 = f2 && (k2 < threshold) && ((k2 & 7u) == 0u);
      bool pred3 = f3 && (k3 < threshold) && ((k3 & 7u) == 0u);
      
      // Conditional accumulation using multiplication by boolean
      sum += static_cast<uint64_t>(payloads[i]) * pred0;
      sum += static_cast<uint64_t>(payloads[i + 1]) * pred1;
      sum += static_cast<uint64_t>(payloads[i + 2]) * pred2;
      sum += static_cast<uint64_t>(payloads[i + 3]) * pred3;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = keys[i];
      uint8_t f = flags[i];
      bool pred = f && (k < threshold) && ((k & 7u) == 0u);
      sum += static_cast<uint64_t>(payloads[i]) * pred;
    }
  }
  
  return sum;
}