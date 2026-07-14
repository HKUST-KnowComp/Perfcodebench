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
  
  // Pre-fetch pointers to avoid repeated vector overhead in the loop
  const uint32_t* __restrict__ k_ptr = keys.data();
  const uint8_t* __restrict__ f_ptr = flags.data();
  const uint32_t* __restrict__ p_ptr = payloads.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    // Single pass: combine predicate check and accumulation
    // This avoids the overhead of std::vector::push_back and the second loop
    for (size_t i = 0; i < n; ++i) {
      // The predicate: flags[i] != 0 AND keys[i] < threshold AND (keys[i] % 8 == 0)
      // We use bitwise AND for the modulo check to assist the compiler
      if (f_ptr[i] && k_ptr[i] < threshold && ((k_ptr[i] & 7u) == 0u)) {
        current_iter_sum += static_cast<uint64_t>(p_ptr[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  
  return total_sum;
}