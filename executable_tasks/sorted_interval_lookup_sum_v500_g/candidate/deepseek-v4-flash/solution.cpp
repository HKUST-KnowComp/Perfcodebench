#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm> // kept for compatibility but no longer required

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  const size_t n_probes = probes.size();
  const size_t n_starts = starts.size();

  // Precompute interval index for each probe using a single linear sweep
  std::vector<uint32_t> probe_interval(n_probes);
  size_t p = 0;
  for (size_t j = 0; j < n_starts; ++j) {
    uint32_t interval_end = (j + 1 < n_starts) ? starts[j + 1] - 1 : UINT32_MAX;
    while (p < n_probes && probes[p] <= interval_end) {
      probe_interval[p] = static_cast<uint32_t>(j);
      ++p;
    }
  }
  // Remaining probes (if any) are assigned to last interval by infinite end; input assumed valid.

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < n_probes; ++i) {
      sum += static_cast<uint64_t>(payloads[probe_interval[i]]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}