#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Create a bitmap of the key space, initialized to false.
  std::vector<uint8_t> bitmap(key_space, 0);

  // Mark enabled keys.
  for (uint32_t k : enabled_keys) {
    bitmap[k] = 1;
  }

  uint64_t sum = 0;
  const std::size_t n = probe_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (bitmap[probe_keys[i]]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}
