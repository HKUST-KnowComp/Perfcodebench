#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  std::vector<uint32_t> selected;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    selected.clear();
    for (std::size_t i = 0; i < keys.size(); ++i) {
      if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
        selected.push_back(values[i]);
      }
    }
    sum = 0;
    for (uint32_t v : selected) {
      sum += static_cast<uint64_t>(v);
    }
  }
  return sum;
}
