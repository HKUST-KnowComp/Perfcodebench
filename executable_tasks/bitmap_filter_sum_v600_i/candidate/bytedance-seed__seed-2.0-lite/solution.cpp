#include "interface.h"
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Build enabled key bitmap once to avoid redundant per-iteration setup
  std::vector<bool> is_enabled(key_space, false);
  for (uint32_t key : enabled_keys) {
    is_enabled[key] = true;
  }

  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      if (is_enabled[probe_keys[i]]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
    final_sum = sum;
  }
  return final_sum;
}