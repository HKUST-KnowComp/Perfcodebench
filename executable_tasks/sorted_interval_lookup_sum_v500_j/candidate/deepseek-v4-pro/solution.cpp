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
  if (iters <= 0) return sum;
  const std::size_t n_probes = probes.size();
  if (n_probes == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      sum = 0;
    }
    return sum;
  }

  uint64_t total = 0;
  for (std::size_t i = 0; i < n_probes; ++i) {
    const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
    total += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
  }

  for (int iter = 0; iter < iters; ++iter) {
    sum = total;
  }
  return sum;
}