#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  // Use a vector of bools as a fast bitset for O(1) lookups.
  // Pre-allocating based on key_space avoids repeated allocations.
  std::vector<uint8_t> enabled(key_space, 0);
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the bitset for the current iteration
    std::fill(enabled.begin(), enabled.end(), 0);
    for (uint32_t key : enabled_keys) {
      if (key < key_space) {
        enabled[key] = 1;
      }
    }

    sum = 0;
    const size_t n = probe_keys.size();
    for (size_t i = 0; i < n; ++i) {
      if (probe_keys[i] < key_space && enabled[probe_keys[i]]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}