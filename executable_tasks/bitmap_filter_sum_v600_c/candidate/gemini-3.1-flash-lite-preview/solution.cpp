#include "interface.h"
#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Use a vector of bytes as a direct-address table for O(1) lookups.
  // key_space is the upper bound of keys.
  std::vector<uint8_t> enabled(key_space + 1, 0);
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the bitmap
    std::fill(enabled.begin(), enabled.end(), 0);
    for (uint32_t key : enabled_keys) {
      if (key <= key_space) {
        enabled[key] = 1;
      }
    }

    sum = 0;
    const size_t n = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_vals = values.data();
    const uint8_t* p_enabled = enabled.data();

    // Tight loop for summation
    for (size_t i = 0; i < n; ++i) {
      if (p_enabled[p_keys[i]]) {
        sum += static_cast<uint64_t>(p_vals[i]);
      }
    }
  }
  return sum;
}