#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {

  // number of 64-bit words needed to store key_space bits
  const uint32_t n64 = (key_space + 63) >> 6;
  std::vector<uint64_t> bitmap(n64, 0);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {

    // clear bitmap in O(n64) instead of clearing a hash-table
    for (uint32_t i = 0; i < n64; ++i) bitmap[i] = 0;

    // mark all enabled keys in the bitmap
    for (uint32_t k : enabled_keys) {
      bitmap[k >> 6] |= (uint64_t(1) << (k & 63));
    }

    // aggregate values whose probe_key is enabled
    sum = 0;
    const std::size_t n = probe_keys.size();
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t pk = probe_keys[i];
      const uint64_t bit = bitmap[pk >> 6] & (uint64_t(1) << (pk & 63));
      sum += (bit != 0) * static_cast<uint64_t>(values[i]);
    }
  }
  return sum;
}