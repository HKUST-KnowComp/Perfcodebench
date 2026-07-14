#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = keys.size();
  
  // Optimization: Perform the filtering and summation in a single pass.
  // Avoid heap allocations inside the loop.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Combine conditions to allow compiler to optimize branch prediction
      if (flags[i] && (keys[i] < threshold) && ((keys[i] & 7u) == 0u)) {
        current_iter_sum += static_cast<uint64_t>(payloads[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}