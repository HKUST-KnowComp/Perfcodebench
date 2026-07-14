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

  std::vector<uint64_t> bitmap;
  if (key_space > 0) {
    bitmap.resize((key_space + 63) / 64, 0);
    for (uint32_t key : enabled_keys) {
      if (key < key_space) {
        bitmap[key >> 6] |= (uint64_t(1) << (key & 63));
      }
    }
  }

  uint64_t sum = 0;
  const std::size_t n = probe_keys.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t k = probe_keys[i];
    if (k < key_space && (bitmap[k >> 6] & (uint64_t(1) << (k & 63)))) {
      sum += values[i];
    }
  }

  return sum;
}