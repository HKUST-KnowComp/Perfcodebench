#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const size_t n = keys.size();
  const uint32_t* __restrict k_ptr = keys.data();
  const uint8_t* __restrict f_ptr = flags.data();
  const uint32_t* __restrict p_ptr = payloads.data();
  
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    #pragma omp simd reduction(+:current_sum)
    for (size_t i = 0; i < n; ++i) {
      // Combine conditions to allow compiler to use bitwise logic/predication
      bool match = (f_ptr[i] != 0) & (k_ptr[i] < threshold) & ((k_ptr[i] & 7u) == 0u);
      current_sum += (match ? static_cast<uint64_t>(p_ptr[i]) : 0ULL);
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}