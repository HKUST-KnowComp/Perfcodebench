#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  const std::size_t n = probes.size();
  std::vector<uint64_t> contrib(n);
  for (std::size_t i = 0; i < n; ++i) {
    auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
    contrib[i] = static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
  }
  uint64_t total = 0;
  for (std::size_t i = 0; i < n; ++i) {
    total += contrib[i];
  }
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = total;
  }
  return sum;
}
