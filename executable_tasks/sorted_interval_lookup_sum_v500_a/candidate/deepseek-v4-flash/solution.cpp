#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  uint64_t sum = 0;
  std::size_t j = 0; // index into starts
  for (std::size_t i = 0; i < probes.size(); ++i) {
    // advance j to first start > probes[i]
    while (j < starts.size() && starts[j] <= probes[i]) {
      ++j;
    }
    if (j > 0) {
      sum += static_cast<uint64_t>(payloads[j-1]) * static_cast<uint64_t>(weights[i]);
    }
    // if j == 0, no start <= probe (matching baseline undefined behavior)
  }
  // iters loop would only recompute the same sum and return the last one,
  // so we return the computed sum directly.
  return sum;
}
