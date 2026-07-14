#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Use a bitset (vector<bool> is specialized for space efficiency and speed)
  // to allow O(1) lookup with minimal cache misses.
  std::vector<bool> bitmap(key_space, false);
  
  // Pre-build the bitmap once outside the loop if enabled_keys doesn't change.
  // Based on the task description, enabled_keys is constant across iterations.
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key] = true;
    }
  }

  uint64_t total_sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_vals = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    // Unrolling or SIMD could be applied here, but a tight loop with 
    // bitset lookup is usually sufficient for median latency targets.
    for (size_t i = 0; i < n; ++i) {
      if (bitmap[p_keys[i]]) {
        current_iter_sum += static_cast<uint64_t>(p_vals[i]);
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}