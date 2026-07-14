#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = keys.size();
  
  // Cache pointers to avoid repeated vector overhead
  const uint32_t* __restrict k_ptr = keys.data();
  const uint8_t* __restrict f_ptr = flags.data();
  const uint32_t* __restrict p_ptr = payloads.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    #pragma omp simd reduction(+:iter_sum)
    for (size_t i = 0; i < n; ++i) {
      // Combine conditions to allow branchless execution or efficient masking
      if (f_ptr[i] && k_ptr[i] < threshold && (k_ptr[i] % 8 == 0)) {
        iter_sum += static_cast<uint64_t>(p_ptr[i]);
      }
    }
    total_sum = iter_sum;
  }
  return total_sum;
}