#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  (void)iters; // result independent of iteration count
  if (key_space == 0) {
    return 0;
  }
  const size_t words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(words, 0);
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key >> 6] |= (static_cast<uint64_t>(1) << (key & 63));
    }
  }
  uint64_t sum = 0;
  for (size_t i = 0; i < probe_keys.size(); ++i) {
    uint32_t key = probe_keys[i];
    if (key < key_space) {
      const uint64_t word = bitmap[key >> 6];
      const uint64_t mask = static_cast<uint64_t>(1) << (key & 63);
      if (word & mask) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}