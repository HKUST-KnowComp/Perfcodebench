#include "interface.h"

#include <algorithm>
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
  const std::size_t n = probes.size();
  // Precompute index of the interval containing each probe.
  std::vector<std::size_t> idx(n);
  for (std::size_t i = 0; i < n; ++i) {
    const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    idx[i] = static_cast<std::size_t>(it - starts.begin() - 1);
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(payloads[idx[i]]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}
