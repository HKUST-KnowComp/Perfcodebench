#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  // Use a vector of bools as a direct-address bitmap for O(1) lookups.
  // This avoids the overhead of hashing and dynamic allocations inside the loop.
  std::vector<bool> bitmap(key_space + 1, false);
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitmap for the current iteration
    std::fill(bitmap.begin(), bitmap.end(), false);
    for (uint32_t key : enabled_keys) {
      if (key <= key_space) {
        bitmap[key] = true;
      }
    }

    uint64_t current_sum = 0;
    const size_t n = probe_keys.size();
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = probe_keys[i];
      if (key <= key_space && bitmap[key]) {
        current_sum += static_cast<uint64_t>(values[i]);
      }
    }
    sum = current_sum;
  }
  
  return sum;
}