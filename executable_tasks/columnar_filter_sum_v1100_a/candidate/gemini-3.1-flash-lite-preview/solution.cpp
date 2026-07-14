#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = keys.size();
  
  // Optimization: Fuse filtering and summation into a single pass.
  // Avoid heap allocation (std::vector) inside the loop.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Combine conditions to allow compiler to optimize branch prediction
      if (flags[i] && (keys[i] < key_threshold) && ((keys[i] & 3u) == 0u)) {
        current_iter_sum += static_cast<uint64_t>(values[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}