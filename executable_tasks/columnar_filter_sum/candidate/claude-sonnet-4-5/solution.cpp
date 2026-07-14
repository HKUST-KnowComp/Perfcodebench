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
    
    // Fused filter and sum - single pass, no intermediate storage
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t key = keys[i];
      // Combine conditions to reduce branches
      // flags[i] is already 0 or 1, use as multiplier
      const bool passes = flags[i] && (key < key_threshold) && ((key & 3u) == 0u);
      // Branchless accumulation using conditional move semantics
      sum += passes ? static_cast<uint64_t>(values[i]) : 0;
    }
  }
  
  return sum;
}