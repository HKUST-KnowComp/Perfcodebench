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
  
  // The logic is invariant across iterations, so we compute once.
  // If iters > 1, we multiply the result by iters.
  for (size_t i = 0; i < n; ++i) {
    if (flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
      total_sum += static_cast<uint64_t>(payloads[i]);
    }
  }
  
  return total_sum * static_cast<uint64_t>(iters);
}