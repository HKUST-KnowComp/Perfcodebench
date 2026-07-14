#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  std::vector<uint8_t> enabled(static_cast<std::size_t>(key_space), 0);
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(enabled.begin(), enabled.end(), 0);
    for (uint32_t key : enabled_keys) {
      enabled[static_cast<std::size_t>(key)] = 1;
    }
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      if (enabled[static_cast<std::size_t>(probe_keys[i])]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}
