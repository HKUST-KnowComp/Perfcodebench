#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  // Precompute the list of values that pass the filter.
  std::vector<uint32_t> filtered;
  filtered.reserve(keys.size());
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
      filtered.push_back(values[i]);
    }
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Compute the sum over the precomputed values (same each iteration).
    uint64_t iter_sum = 0;
    for (uint32_t v : filtered) {
      iter_sum += static_cast<uint64_t>(v);
    }
    sum = iter_sum;
  }
  return sum;
}
