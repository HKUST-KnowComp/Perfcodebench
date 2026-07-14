#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = keys.size();
  // Precompute input size once to avoid repeated vector size checks
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    // Fused filter and sum: no intermediate storage, single pass over input
    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}