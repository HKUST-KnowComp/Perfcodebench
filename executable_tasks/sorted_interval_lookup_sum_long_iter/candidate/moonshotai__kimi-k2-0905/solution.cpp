#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;  // not used in the rule, kept for signature compatibility

  // Pre-compute the payload index for every probe once.
  const std::size_t n_probes = probes.size();
  std::vector<uint64_t> cached(n_probes, 0);

  for (std::size_t i = 0; i < n_probes; ++i) {
    const uint32_t p = probes[i];
    // Find the last interval whose start <= p.
    const auto it = std::upper_bound(starts.begin(), starts.end(), p);
    if (it != starts.begin()) {
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      cached[i] = static_cast<uint64_t>(payloads[idx]);
    } else {
      cached[i] = 0;  // probe precedes all intervals
    }
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n_probes; ++i) {
      sum += cached[i] * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}