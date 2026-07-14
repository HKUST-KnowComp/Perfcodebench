#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // The reference result is independent of `iters` because inputs are immutable.
  if (iters <= 0 || enabled_keys.empty() || probe_keys.empty()) {
    return 0;
  }

  // Build a dense byte bitmap for O(1) membership tests.
  std::vector<uint8_t> bitmap(static_cast<std::size_t>(key_space), 0);
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key] = 1;
    }
  }

  const std::size_t n = probe_keys.size();
  const uint32_t* probe_data = probe_keys.data();
  const uint32_t* values_data = values.data();
  const uint8_t* bitmap_data = bitmap.data();

  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t key = probe_data[i];
    if (key < key_space && bitmap_data[key]) {
      sum += static_cast<uint64_t>(values_data[i]);
    }
  }

  return sum;
}
