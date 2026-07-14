#include "interface.h"

#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t idx = 0;
    for (std::size_t i = 0; i < probes.size(); ++i) {
      const auto it = std::upper_bound(starts.begin() + idx, starts.end(), probes[i]);
      idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}