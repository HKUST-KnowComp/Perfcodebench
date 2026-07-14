#include "interface.h"

#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  const uint32_t* s = starts.data();
  const uint32_t* e = ends.data();
  const uint32_t* pay = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();
  const std::size_t n = starts.size();
  const std::size_t m = probes.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    if (n == 0 || m == 0) {
      continue;
    }

    std::size_t idx = 0; // current interval index
    for (std::size_t i = 0; i < m; ++i) {
      const uint32_t p = pr[i];

      // Advance interval index while current interval ends before the probe
      while (idx < n && e[idx] < p) {
        ++idx;
      }
      if (idx == n) {
        // No more intervals can match remaining (sorted) probes
        break;
      }

      // If probe is within [start, end], accumulate weighted payload
      if (p >= s[idx]) {
        sum += static_cast<uint64_t>(pay[idx]) * static_cast<uint64_t>(w[i]);
      }
      // else: probe is before the next interval start; contributes nothing
    }
  }
  return sum;
}
