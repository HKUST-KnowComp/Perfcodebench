#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  const size_t n = keys.size();
  
  // Pre-fetch pointers to avoid repeated std::vector overhead in the loop
  const uint32_t* __restrict__ k_ptr = keys.data();
  const uint32_t* __restrict__ v_ptr = values.data();
  const uint8_t* __restrict__ f_ptr = flags.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    // Single pass fusion: filter and sum simultaneously
    // This avoids the allocation and memory traffic of the 'selected' vector
    for (size_t i = 0; i < n; ++i) {
      // Order of conditions optimized for early exit
      // flags[i] is checked first as it is often a sparse bitmask
      if (f_ptr[i] && k_ptr[i] < key_threshold && ((k_ptr[i] & 3u) == 0u)) {
        current_sum += static_cast<uint64_t>(v_ptr[i]);
      }
    }
    sum = current_sum;
  }
  
  return sum;
}