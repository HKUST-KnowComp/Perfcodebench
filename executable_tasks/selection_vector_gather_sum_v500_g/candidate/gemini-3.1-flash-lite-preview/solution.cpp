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
  
  // The original code performed the loop 'iters' times but only returned the sum from the last iteration.
  // We maintain this behavior while optimizing the inner loop.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    const uint32_t* k_ptr = keys.data();
    const uint8_t* f_ptr = flags.data();
    const uint32_t* p_ptr = payloads.data();

    for (size_t i = 0; i < n; ++i) {
      // Predicate: flags[i] is true AND keys[i] < threshold AND keys[i] is multiple of 8
      if (f_ptr[i] && k_ptr[i] < threshold && (k_ptr[i] & 7u) == 0u) {
        current_sum += static_cast<uint64_t>(p_ptr[i]);
      }
    }
    total_sum = current_sum;
  }
  return total_sum;
}