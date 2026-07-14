#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) return 0;

  // Use a dense bitset (vector of bool or uint8_t) for O(1) lookup.
  // uint8_t is often faster than std::vector<bool> due to avoiding bit-manipulation overhead.
  std::vector<uint8_t> bitmap(key_space, 0);
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitmap
    for (uint32_t k : enabled_keys) {
      bitmap[k] = 1;
    }

    uint64_t current_iter_sum = 0;
    const size_t n = probe_keys.size();
    const uint32_t* __restrict p_keys = probe_keys.data();
    const uint32_t* __restrict p_vals = values.data();

    // Tight loop for probing
    for (size_t i = 0; i < n; ++i) {
      if (bitmap[p_keys[i]]) {
        current_iter_sum += static_cast<uint64_t>(p_vals[i]);
      }
    }
    
    total_sum = current_iter_sum;

    // Clean up bitmap for next iteration
    for (uint32_t k : enabled_keys) {
      bitmap[k] = 0;
    }
  }

  return total_sum;
}