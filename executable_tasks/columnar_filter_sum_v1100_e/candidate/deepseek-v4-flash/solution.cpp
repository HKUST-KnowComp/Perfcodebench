#include "interface.h"
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
      sum += static_cast<uint64_t>(values[i]);
    }
  }
  return sum;
}