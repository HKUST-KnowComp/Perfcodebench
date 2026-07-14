#include "interface.h"

#include <vector>
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  // Pre-compute interval indices for all probes (only once)
  std::vector<std::size_t> probe_indices(probes.size());
  for (std::size_t i = 0; i < probes.size(); ++i) {
    const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    probe_indices[i] = static_cast<std::size_t>(it - starts.begin() - 1);
  }
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < probes.size(); ++i) {
      sum += static_cast<uint64_t>(payloads[probe_indices[i]]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}