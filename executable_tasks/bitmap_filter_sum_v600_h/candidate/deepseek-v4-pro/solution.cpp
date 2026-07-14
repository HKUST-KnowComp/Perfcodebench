#include "interface.h"

#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) return 0;
  std::vector<bool> bitmap(key_space, false);
  for (uint32_t k : enabled_keys) {
    bitmap[k] = true;
  }
  uint64_t sum = 0;
  for (std::size_t i = 0; i < probe_keys.size(); ++i) {
    uint32_t key = probe_keys[i];
    if (key < key_space && bitmap[key]) {
      sum += static_cast<uint64_t>(values[i]);
    }
  }
  return sum;
}
