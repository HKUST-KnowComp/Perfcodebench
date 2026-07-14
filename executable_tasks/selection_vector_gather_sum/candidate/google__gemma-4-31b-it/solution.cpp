#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const size_t n = keys.size();
  const uint32_t* __restrict__ k_ptr = keys.data();
  const uint8_t* __restrict__ f_ptr = flags.data();
  const uint32_t* __restrict__ p_ptr = payloads.data();
  
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Combine predicates to allow compiler to optimize branching/masking
      // flags[i] is non-zero, keys[i] < threshold, and keys[i] is multiple of 8
      if (f_ptr[i] && k_ptr[i] < threshold && (k_ptr[i] & 7u) == 0u) {
        current_sum += p_ptr[i];
      }
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}