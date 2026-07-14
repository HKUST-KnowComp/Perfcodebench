#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  // Use a bitset represented by a vector of uint8_t for O(1) lookup.
  // key_space is the upper bound of keys.
  std::vector<uint8_t> bitmap(key_space + 1, 0);
  
  for (uint32_t key : enabled_keys) {
    if (key <= key_space) {
      bitmap[key] = 1;
    }
  }

  uint64_t total_sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_vals = values.data();
  const uint8_t* b_map = bitmap.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      if (b_map[p_keys[i]]) {
        current_sum += static_cast<uint64_t>(p_vals[i]);
      }
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}