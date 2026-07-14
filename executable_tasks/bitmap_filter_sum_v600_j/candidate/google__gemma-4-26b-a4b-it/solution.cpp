#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Use a bitset (vector<bool> is specialized for space/speed) to represent the enabled keys.
  // This provides O(1) lookup with much better cache locality than unordered_set.
  std::vector<bool> bitmap(key_space, false);
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitmap for each iteration
    std::fill(bitmap.begin(), bitmap.end(), false);
    
    // Populate bitmap
    for (uint32_t key : enabled_keys) {
      if (key < key_space) {
        bitmap[key] = true;
      }
    }

    uint64_t current_iter_sum = 0;
    const size_t n = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_vals = values.data();

    // Tight loop for probing
    for (size_t i = 0; i < n; ++i) {
      uint32_t k = p_keys[i];
      // Check bounds and membership
      if (k < key_space && bitmap[k]) {
        current_iter_sum += static_cast<uint64_t>(p_vals[i]);
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}