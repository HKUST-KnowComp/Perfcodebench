#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {

  // Build a fast membership bitmap once
  std::vector<uint8_t> is_enabled(key_space, 0);
  for (uint32_t k : enabled_keys) {
    if (k < key_space) is_enabled[k] = 1;
  }

  uint64_t sum = 0;
  const std::size_t n = probe_keys.size();
  const uint32_t* probe = probe_keys.data();
  const uint32_t* vals = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (is_enabled[probe[i]]) {
        sum += static_cast<uint64_t>(vals[i]);
      }
    }
  }
  return sum;
}
