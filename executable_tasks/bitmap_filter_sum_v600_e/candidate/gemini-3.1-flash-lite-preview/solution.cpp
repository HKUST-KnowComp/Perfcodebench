#include "interface.h"
#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  // Use a vector of bools or bytes as a direct-address table for O(1) lookups.
  // Since key_space is provided, we can allocate once.
  std::vector<uint8_t> enabled(key_space, 0);
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the bitmap
    std::fill(enabled.begin(), enabled.end(), 0);
    
    // Mark enabled keys
    for (uint32_t key : enabled_keys) {
      if (key < key_space) {
        enabled[key] = 1;
      }
    }

    // Calculate sum
    sum = 0;
    const size_t n = probe_keys.size();
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = probe_keys[i];
      if (key < key_space && enabled[key]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}