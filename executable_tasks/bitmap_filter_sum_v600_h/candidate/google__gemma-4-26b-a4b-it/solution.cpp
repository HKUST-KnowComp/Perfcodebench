#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  // Use a bitset for O(1) lookup. 
  // A vector<bool> is specialized in C++ to be space-efficient (1 bit per element).
  // Given key_space can be large, we allocate once.
  std::vector<bool> bitmap(key_space, false);
  
  // Pre-calculate the bitmap once if enabled_keys doesn't change per iteration.
  // Based on the baseline, enabled_keys is constant across 'iters'.
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
    // Unrolling or SIMD could be applied here, but a simple loop over 
    // a bitset is already extremely fast due to cache locality.
    for (size_t i = 0; i < n; ++i) {
      if (bitmap[p_keys[i]]) {
        current_iter_sum += static_cast<uint64_t>(p_vals[i]);
      }
    }
    total_sum = current_iter_sum; // The baseline returns the sum of the LAST iteration
  }

  return total_sum;
}