#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (key_space == 0 || probe_keys.empty()) return 0;

  size_t bitmap_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(bitmap_words, 0);
  uint64_t sum = 0;

  const uint32_t* pk = probe_keys.data();
  const uint32_t* val = values.data();
  size_t n = probe_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t k : enabled_keys) {
      bitmap[k >> 6] |= (1ULL << (k & 63));
    }

    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t k = pk[i];
      if (bitmap[k >> 6] & (1ULL << (k & 63))) {
        current_sum += val[i];
      }
    }

    for (uint32_t k : enabled_keys) {
      bitmap[k >> 6] &= ~(1ULL << (k & 63));
    }
    sum = current_sum;
  }
  return sum;
}