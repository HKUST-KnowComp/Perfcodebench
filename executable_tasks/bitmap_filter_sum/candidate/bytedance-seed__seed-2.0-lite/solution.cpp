#include "interface.h"
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Build presence bitmap once before all iterations to avoid redundant setup
  std::vector<bool> enabled(key_space, false);
  for (uint32_t key : enabled_keys) {
    enabled[key] = true;
  }

  uint64_t final_sum = 0;
  const size_t probe_count = probe_keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    final_sum = 0;
    for (size_t i = 0; i < probe_count; ++i) {
      if (enabled[probe_keys[i]]) {
        final_sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return final_sum;
}