#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t sum = 0;
  const std::size_t size = keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
    // Unrolled loop for better ILP and reduced branch overhead
    for (; i + 3 < size; i += 4) {
      // Process 4 elements at once
      const uint32_t k0 = keys[i];
      const uint32_t k1 = keys[i + 1];
      const uint32_t k2 = keys[i + 2];
      const uint32_t k3 = keys[i + 3];
      
      const uint8_t f0 = flags[i];
      const uint8_t f1 = flags[i + 1];
      const uint8_t f2 = flags[i + 2];
      const uint8_t f3 = flags[i + 3];
      
      // Branchless predicate evaluation
      const bool pred0 = f0 && (k0 < threshold) && ((k0 & 7u) == 0u);
      const bool pred1 = f1 && (k1 < threshold) && ((k1 & 7u) == 0u);
      const bool pred2 = f2 && (k2 < threshold) && ((k2 & 7u) == 0u);
      const bool pred3 = f3 && (k3 < threshold) && ((k3 & 7u) == 0u);
      
      // Conditional accumulation (compiler will optimize to CMOV)
      if (pred0) sum += static_cast<uint64_t>(payloads[i]);
      if (pred1) sum += static_cast<uint64_t>(payloads[i + 1]);
      if (pred2) sum += static_cast<uint64_t>(payloads[i + 2]);
      if (pred3) sum += static_cast<uint64_t>(payloads[i + 3]);
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      const uint32_t k = keys[i];
      if (flags[i] && (k < threshold) && ((k & 7u) == 0u)) {
        sum += static_cast<uint64_t>(payloads[i]);
      }
    }
  }
  
  return sum;
}