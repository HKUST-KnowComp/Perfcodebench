#include "interface.h"

#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  std::vector<bool> enabled(key_space, false);
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (const auto key : enabled_keys) {
      enabled[key] = true;
    }
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      if (probe_keys[i] < key_space && enabled[probe_keys[i]]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
    for (const auto key : enabled_keys) {
      enabled[key] = false;
    }
  }
  return sum;
}