#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  const std::size_t n = probe_keys.size();
  if (iters <= 0 || n == 0 || enabled_keys.empty() || key_space == 0) {
    return 0;
  }

  const std::size_t word_count = (static_cast<std::size_t>(key_space) + 63u) >> 6;
  std::vector<uint64_t> bitmap(word_count, 0);

  for (uint32_t k : enabled_keys) {
    if (k < key_space) {
      bitmap[static_cast<std::size_t>(k) >> 6] |= (uint64_t{1} << (k & 63u));
    }
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t k = probe_keys[i];
      if (k < key_space) {
        local_sum += static_cast<uint64_t>(values[i]) *
                     ((bitmap[static_cast<std::size_t>(k) >> 6] >> (k & 63u)) & 1u);
      }
    }
    sum = local_sum;
  }
  return sum;
}
