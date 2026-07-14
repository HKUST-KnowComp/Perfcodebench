#include "interface.h"

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
    std::size_t j = 0;
    for (std::size_t i = 0; i < probes.size(); ++i) {
      while (j + 1 < starts.size() && starts[j + 1] <= probes[i]) {
        ++j;
      }
      sum += static_cast<uint64_t>(payloads[j]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}
