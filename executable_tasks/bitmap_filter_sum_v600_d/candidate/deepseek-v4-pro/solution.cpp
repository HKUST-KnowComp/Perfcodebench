#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0) return 0;

  const size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bits(num_words, 0);

  // Build the bitmap once
  for (uint32_t k : enabled_keys) {
    if (k < key_space) {
      size_t idx = k >> 6;
      uint64_t mask = static_cast<uint64_t>(1) << (k & 63);
      bits[idx] |= mask;
    }
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < probe_keys.size(); ++i) {
      uint32_t k = probe_keys[i];
      if (k < key_space) {
        size_t idx = k >> 6;
        uint64_t mask = static_cast<uint64_t>(1) << (k & 63);
        if (bits[idx] & mask) {
          sum += static_cast<uint64_t>(values[i]);
        }
      }
    }
  }

  return sum;
}