#include "interface.h"

#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  (void)iters;
  std::vector<bool> enabled(key_space, false);
  for (uint32_t key : enabled_keys) {
    enabled[key] = true;
  }

  uint64_t sum = 0;
  const std::size_t n = probe_keys.size();
  for (std::size_t i = 0; i < n; ++i) {
    if (enabled[probe_keys[i]]) {
      sum += static_cast<uint64_t>(values[i]);
    }
  }
  return sum;
}
