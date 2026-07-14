#include "interface.h"
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters == 0) return 0;
  std::vector<unsigned char> bitmap(key_space, 0);
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key] = 1;
    }
  }
  uint64_t sum = 0;
  for (std::size_t i = 0; i < probe_keys.size(); ++i) {
    uint32_t k = probe_keys[i];
    if (k < key_space && bitmap[k]) {
      sum += static_cast<uint64_t>(values[i]);
    }
  }
  return sum;
}