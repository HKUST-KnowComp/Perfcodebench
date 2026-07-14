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

  if (probe_keys.empty() || key_space == 0) {
    return 0;
  }

  const std::size_t nbytes = (static_cast<std::size_t>(key_space) + 7u) >> 3;
  std::vector<uint8_t> bitmap(nbytes, 0u);

  for (uint32_t k : enabled_keys) {
    if (k < key_space) {
      bitmap[k >> 3] |= static_cast<uint8_t>(1u << (k & 7));
    }
  }

  uint64_t sum = 0;
  const uint32_t* pk = probe_keys.data();
  const uint32_t* vals = values.data();
  const std::size_t n = probe_keys.size();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t k = pk[i];
    if (k < key_space) {
      if (bitmap[k >> 3] & static_cast<uint8_t>(1u << (k & 7))) {
        sum += static_cast<uint64_t>(vals[i]);
      }
    }
  }

  return sum;
}
