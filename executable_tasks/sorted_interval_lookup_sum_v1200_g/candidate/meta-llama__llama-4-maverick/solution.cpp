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
    auto starts_begin = starts.begin();
    for (std::size_t i = 0; i < probes.size(); ++i) {
      const auto it = std::lower_bound(starts_begin, starts.end(), probes[i]);
      if (it != starts_begin) {
        const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
        if (probes[i] <= ends[idx]) {
          sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
        }
      }
      starts_begin = it;
    }
  }
  return sum;
}