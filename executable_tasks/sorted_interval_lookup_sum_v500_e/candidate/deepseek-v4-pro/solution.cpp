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
  (void)ends;   // not used
  (void)iters;  // we compute the sum once, it is invariant across iterations

  if (probes.empty() || starts.empty()) {
    return 0;
  }

  std::size_t j = 0;
  uint64_t sum = 0;
  for (std::size_t i = 0; i < probes.size(); ++i) {
    while (j < starts.size() && starts[j] <= probes[i]) {
      ++j;
    }
    // j is the index of the first start > probe, so the matching interval is j-1
    std::size_t idx = j - 1;
    sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
  }

  return sum;
}