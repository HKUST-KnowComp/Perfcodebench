#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  std::vector<uint32_t> selected;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    selected.clear();
    for (std::size_t i = 0; i < keys.size(); ++i) {
      if (flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
        selected.push_back(static_cast<uint32_t>(i));
      }
    }
    sum = 0;
    for (uint32_t idx : selected) {
      sum += static_cast<uint64_t>(payloads[static_cast<std::size_t>(idx)]);
    }
  }
  return sum;
}
