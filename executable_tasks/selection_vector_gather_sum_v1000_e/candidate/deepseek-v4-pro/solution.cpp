#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  (void)iters;
  uint64_t sum = 0;
  const size_t n = keys.size();
  for (size_t i = 0; i < n; ++i) {
    if (flags[i] && keys[i] < threshold && !(keys[i] & 7u)) {
      sum += payloads[i];
    }
  }
  return sum;
}
