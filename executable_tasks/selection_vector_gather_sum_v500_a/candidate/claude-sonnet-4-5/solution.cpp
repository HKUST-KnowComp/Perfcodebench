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
      // Prefetch next cache lines
      __builtin_prefetch(&keys[i + 16], 0, 0);
      __builtin_prefetch(&flags[i + 16], 0, 0);
      
      // Unroll 4 iterations
      uint32_t k0 = keys[i];
      uint32_t k1 = keys[i + 1];
      uint32_t k2 = keys[i + 2];
      uint32_t k3 = keys[i + 3];
      
      uint8_t f0 = flags[i];
      uint8_t f1 = flags[i + 1];
      uint8_t f2 = flags[i + 2];
      uint8_t f3 = flags[i + 3];
      
      // Compute predicates using bitwise ops to avoid branches
      bool pred0 = f0 && (k0 < threshold) && ((k0 & 7u) == 0u);
      bool pred1 = f1 && (k1 < threshold) && ((k1 & 7u) == 0u);
      bool pred2 = f2 && (k2 < threshold) && ((k2 & 7u) == 0u);
      bool pred3 = f3 && (k3 < threshold) && ((k3 & 7u) == 0u);
      
      // Conditional accumulation using branchless selection
      sum += pred0 ? static_cast<uint64_t>(payloads[i]) : 0;
      sum += pred1 ? static_cast<uint64_t>(payloads[i + 1]) : 0;
      sum += pred2 ? static_cast<uint64_t>(payloads[i + 2]) : 0;
      sum += pred3 ? static_cast<uint64_t>(payloads[i + 3]) : 0;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = keys[i];
      if (flags[i] && (k < threshold) && ((k & 7u) == 0u)) {
        sum += static_cast<uint64_t>(payloads[i]);
      }
    }
  }
  
  return sum;
}