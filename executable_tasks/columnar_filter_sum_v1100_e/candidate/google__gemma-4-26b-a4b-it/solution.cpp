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
  const uint32_t* k_ptr = keys.data();
  const uint32_t* v_ptr = values.data();
  const uint8_t* f_ptr = flags.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Fuse filtering and summation to avoid temporary vector allocation and extra pass
      // The condition (keys[i] & 3u) == 0u is checked last to potentially short-circuit
      if (f_ptr[i] && k_ptr[i] < key_threshold && ((k_ptr[i] & 3u) == 0u)) {
        current_sum += static_cast<uint64_t>(v_ptr[i]);
      }
    }
    sum = current_sum;
  }
  return sum;
}