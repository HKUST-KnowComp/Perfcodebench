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
    auto it = starts.begin();
    for (std::size_t i = 0; i < probes.size(); ++i) {
      it = std::lower_bound(it, starts.end(), probes[i]);
      if (it != starts.begin()) {
        --it;
        sum += static_cast<uint64_t>(payloads[it - starts.begin()]) * static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return sum;
}