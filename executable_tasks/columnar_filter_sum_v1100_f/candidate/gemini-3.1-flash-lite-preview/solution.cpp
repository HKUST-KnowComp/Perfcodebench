#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = keys.size();
  
  // Optimization: Fuse filtering and summation into a single pass.
  // Avoid intermediate vector allocation to reduce memory overhead and improve cache locality.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    const uint8_t* f_ptr = flags.data();

    for (size_t i = 0; i < n; ++i) {
      // Check flags and conditions. Using bitwise AND for (keys[i] & 3u) == 0u.
      if (f_ptr[i] && k_ptr[i] < key_threshold && (k_ptr[i] & 3u) == 0u) {
        current_iter_sum += static_cast<uint64_t>(v_ptr[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}