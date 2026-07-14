#include "interface.h"

#include <vector>
#include <cstdint>

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
    size_t interval_idx = 0;
    for (size_t i = 0; i < probes.size(); ++i) {
      const uint32_t probe = probes[i];
      while (interval_idx < starts.size() && starts[interval_idx] <= probe) {
        interval_idx++;
      }
      sum += static_cast<uint64_t>(payloads[interval_idx - 1]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}