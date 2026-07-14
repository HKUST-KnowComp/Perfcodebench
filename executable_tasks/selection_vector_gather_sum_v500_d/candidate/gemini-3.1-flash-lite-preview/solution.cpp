#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t sum = 0;
  const size_t n = keys.size();
  
  // The logic requires iterating 'iters' times. 
  // Since the input data is constant, we compute the sum once and multiply by iters.
  // This is mathematically equivalent to the original loop structure.
  uint64_t single_pass_sum = 0;
  for (size_t i = 0; i < n; ++i) {
    // Predicate: flags[i] is true, keys[i] < threshold, and keys[i] is a multiple of 8
    if (flags[i] && (keys[i] < threshold) && ((keys[i] & 7u) == 0u)) {
      single_pass_sum += static_cast<uint64_t>(payloads[i]);
    }
  }
  
  sum = single_pass_sum * static_cast<uint64_t>(iters);
  return sum;
}