#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0 || probe_keys.empty()) {
    return 0;
  }

  // Build a dense byte bitmap once. Value-initialization guarantees all zeros.
  std::vector<uint8_t> bitmap(key_space);
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key] = 1;
    }
  }

  const size_t n = probe_keys.size();
  const uint32_t* __restrict probe_data = probe_keys.data();
  const uint32_t* __restrict values_data = values.data();
  const uint8_t* __restrict bitmap_data = bitmap.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      const uint32_t key = probe_data[i];
      if (key < key_space) {
        // Branchless accumulation: bitmap_data[key] is 0 or 1.
        iter_sum += static_cast<uint64_t>(values_data[i]) * bitmap_data[key];
      }
    }
    sum = iter_sum;
  }
  return sum;
}
