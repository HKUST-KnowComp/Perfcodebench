#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const size_t n = keys.size();
  const uint32_t* k_ptr = keys.data();
  const uint32_t* v_ptr = values.data();
  const uint8_t* f_ptr = flags.data();
  
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    // Fusing the filter and sum into a single pass to avoid memory allocation
    // and redundant iterations. The conditions are combined to allow the 
    // compiler to optimize the branch or use conditional moves/SIMD.
    for (size_t i = 0; i < n; ++i) {
      if (f_ptr[i] && k_ptr[i] < key_threshold && (k_ptr[i] & 3u) == 0u) {
        current_sum += v_ptr[i];
      }
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}