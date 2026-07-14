#include "interface.h"
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  std::vector<bool> enabled_bitset(key_space, false);
  for (uint32_t key : enabled_keys) {
    enabled_bitset[key] = true;
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      uint32_t key = probe_keys[i];
      if (enabled_bitset[key]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}