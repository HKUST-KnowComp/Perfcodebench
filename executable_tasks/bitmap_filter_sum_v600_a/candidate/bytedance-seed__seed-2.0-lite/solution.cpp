#include "interface.h"

#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Precompute enabled key bitmap once before all iterations to avoid redundant work
  std::vector<bool> is_enabled(key_space, false);
  for (uint32_t key : enabled_keys) {
    is_enabled[key] = true;
  }

  uint64_t sum = 0;
  const std::size_t probe_count = probe_keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    // Linear CPU-prefetchable scan with fast bitmap membership checks
    for (std::size_t i = 0; i < probe_count; ++i) {
      if (is_enabled[probe_keys[i]]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}