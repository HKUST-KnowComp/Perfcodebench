#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) return 0;

  // Use a bitset for O(1) lookup. 
  // std::vector<bool> is space-optimized in most STL implementations.
  std::vector<bool> bitmap(key_space, false);
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key] = true;
    }
  }

  uint64_t total_sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* probe_ptr = probe_keys.data();
  const uint32_t* values_ptr = values.data();

  // The enabled_keys and bitmap construction are invariant across iterations.
  // We perform the probe loop 'iters' times as required by the task.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = probe_ptr[i];
      // Check bounds and bit presence
      if (key < key_space && bitmap[key]) {
        current_iter_sum += static_cast<uint64_t>(values_ptr[i]);
      }
    }
    total_sum = current_iter_sum; // The task implies returning the sum of the last iteration
  }

  return total_sum;
}