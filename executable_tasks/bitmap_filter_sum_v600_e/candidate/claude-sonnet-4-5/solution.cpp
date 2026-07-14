#include "interface.h"

#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  std::vector<bool> bitmap(key_space, false);
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t key : enabled_keys) {
      bitmap[key] = true;
    }
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      if (bitmap[probe_keys[i]]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
    for (uint32_t key : enabled_keys) {
      bitmap[key] = false;
    }
  }
  return sum;
}