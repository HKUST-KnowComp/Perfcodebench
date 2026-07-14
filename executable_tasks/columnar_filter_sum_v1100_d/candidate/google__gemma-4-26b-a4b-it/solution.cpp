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
  
  // Pre-fetch pointers to avoid repeated vector overhead in the loop
  const uint32_t* k_ptr = keys.data();
  const uint32_t* v_ptr = values.data();
  const uint8_t* f_ptr = flags.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    // Single pass: Fuse filter and sum to maximize cache locality and eliminate vector allocation
    for (size_t i = 0; i < n; ++i) {
      // Predicate: flags[i] != 0 AND keys[i] < threshold AND (keys[i] % 4 == 0)
      if (f_ptr[i] && k_ptr[i] < key_threshold && ((k_ptr[i] & 3u) == 0u)) {
        current_iter_sum += static_cast<uint64_t>(v_ptr[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  
  return total_sum;
}