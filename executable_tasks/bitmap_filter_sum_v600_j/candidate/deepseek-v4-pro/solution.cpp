#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Pre-build a byte-map for O(1) membership tests
  std::vector<uint8_t> bitmap(key_space, 0);
  for (uint32_t k : enabled_keys) {
    bitmap[k] = 1;
  }

  const uint32_t* pkeys = probe_keys.data();
  const uint32_t* vals = values.data();
  const size_t n = probe_keys.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < n; ++i) {
      if (bitmap[pkeys[i]]) {
        sum += static_cast<uint64_t>(vals[i]);
      }
    }
  }

  return sum;
}
