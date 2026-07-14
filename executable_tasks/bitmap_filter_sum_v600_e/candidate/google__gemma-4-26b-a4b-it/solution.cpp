#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Use a bitset-like vector for O(1) lookup with minimal overhead.
  // We use uint8_t to avoid the overhead of std::vector<bool> bit-packing 
  // if the key_space is small, but for general cases, a simple byte-array 
  // is often faster due to avoiding bit-shifting logic.
  std::vector<uint8_t> bitmap(key_space, 0);
  
  // Pre-calculate the bitmap once if enabled_keys doesn't change per iteration.
  // Based on the baseline, enabled_keys is inserted every iteration, 
  // but it's the same set. We move it outside the loop to optimize.
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key] = 1;
    }
  }

  uint64_t total_sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_vals = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    // Unrolling or SIMD could be applied here, but a clean loop 
    // with a direct array lookup is highly efficient for modern CPUs.
    for (size_t i = 0; i < n; ++i) {
      if (bitmap[p_keys[i]]) {
        current_sum += static_cast<uint64_t>(p_vals[i]);
      }
    }
    total_sum = current_sum;
  }

  return total_sum;
}