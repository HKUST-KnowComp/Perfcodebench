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
  
  // Accessing raw pointers to avoid vector bounds checking overhead
  const uint32_t* __restrict k_ptr = keys.data();
  const uint32_t* __restrict v_ptr = values.data();
  const uint8_t* __restrict f_ptr = flags.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Combined condition: flag is non-zero, key < threshold, and key is multiple of 4
      if (f_ptr[i] && k_ptr[i] < key_threshold && (k_ptr[i] & 3u) == 0u) {
        current_iter_sum += static_cast<uint64_t>(v_ptr[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}