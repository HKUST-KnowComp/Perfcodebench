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
  const uint32_t* k_ptr = keys.data();
  const uint8_t* f_ptr = flags.data();
  const uint32_t* p_ptr = payloads.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    // Single pass: filter and accumulate simultaneously to avoid vector allocation/resizing
    for (size_t i = 0; i < n; ++i) {
      // Combined predicate check
      // 1. flags[i] is non-zero
      // 2. keys[i] < threshold
      // 3. keys[i] % 8 == 0 (bitmask check is faster than modulo)
      if (f_ptr[i] && k_ptr[i] < threshold && ((k_ptr[i] & 7u) == 0u)) {
        current_iter_sum += static_cast<uint64_t>(p_ptr[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  
  return total_sum;
}