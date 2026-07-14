#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  // Use a vector of bools as a direct-address table for O(1) lookups.
  // This avoids hashing and dynamic allocation overhead inside the loop.
  std::vector<bool> enabled(key_space, false);
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      enabled[key] = true;
    }
  }

  uint64_t total_sum = 0;
  const size_t n = probe_keys.size();
  
  // Cache pointers to raw data for faster access
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_vals = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      if (enabled[p_keys[i]]) {
        current_iter_sum += static_cast<uint64_t>(p_vals[i]);
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}