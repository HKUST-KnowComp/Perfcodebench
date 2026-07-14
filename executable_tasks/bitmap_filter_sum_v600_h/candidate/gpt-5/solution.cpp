#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  (void)iters;

  const std::size_t n = probe_keys.size();
  if (n == 0 || enabled_keys.empty() || key_space == 0) {
    return 0ULL;
  }

  // Build packed bitmap of enabled keys: 1 bit per key in [0, key_space)
  const std::size_t words = (static_cast<std::size_t>(key_space) + 63u) / 64u;
  std::vector<uint64_t> bitmap(words, 0ULL);
  for (uint32_t k : enabled_keys) {
    if (k < key_space) {
      bitmap[k >> 6] |= (uint64_t{1} << (k & 63));
    }
  }

  uint64_t sum = 0ULL;
  const uint64_t* bmp = bitmap.data();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t k = probe_keys[i];
    if (k < key_space) {
      const uint64_t mask = (uint64_t{1} << (k & 63));
      if (bmp[k >> 6] & mask) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }

  return sum;
}