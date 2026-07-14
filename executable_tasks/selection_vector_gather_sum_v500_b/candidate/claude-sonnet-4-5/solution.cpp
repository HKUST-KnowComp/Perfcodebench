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
    
    // Fused selection and accumulation - no intermediate storage
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t key = keys[i];
      const uint8_t flag = flags[i];
      
      // Branchless predicate evaluation
      const bool passes = flag && (key < threshold) && ((key & 7u) == 0u);
      
      // Conditional accumulation - compiler can optimize this better than branching
      sum += passes ? static_cast<uint64_t>(payloads[i]) : 0;
    }
  }
  
  return sum;
}