#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  if (enabled_keys.empty() || probe_keys.empty()) {
    return 0;
  }

  std::vector<uint8_t> bitmap(key_space, 0);
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key] = 1;
    }
  }

  uint64_t sum = 0;
  const size_t n = probe_keys.size();
  for (size_t i = 0; i < n; ++i) {
    uint32_t key = probe_keys[i];
    if (key < key_space && bitmap[key]) {
      sum += static_cast<uint64_t>(values[i]);
    }
  }

  return sum;
}
