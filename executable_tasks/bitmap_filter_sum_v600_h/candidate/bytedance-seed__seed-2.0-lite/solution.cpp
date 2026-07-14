#include "interface.h"
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Prebuild enabled bitmap once to avoid repeated construction overhead
  std::vector<bool> enabled(key_space, false);
  for (uint32_t key : enabled_keys) {
    enabled[key] = true;
  }

  uint64_t sum = 0;
  const size_t probe_len = probe_keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    // Cache-aligned linear scan with direct bitmap lookups for minimal latency
    for (size_t i = 0; i < probe_len; ++i) {
      if (enabled[probe_keys[i]]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}