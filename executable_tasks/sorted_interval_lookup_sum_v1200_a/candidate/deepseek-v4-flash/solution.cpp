#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  const uint64_t n_probes = probes.size();
  const uint64_t n_starts = starts.size();
  // Precompute for each probe the index of the interval it falls into.
  std::vector<uint64_t> idx_of_probe(n_probes);
  uint64_t interval_idx = 0;
  for (uint64_t i = 0; i < n_probes; ++i) {
    while (interval_idx + 1 < n_starts && starts[interval_idx + 1] <= probes[i]) {
      ++interval_idx;
    }
    idx_of_probe[i] = interval_idx;
  }
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint64_t i = 0; i < n_probes; ++i) {
      sum += static_cast<uint64_t>(payloads[idx_of_probe[i]]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}
