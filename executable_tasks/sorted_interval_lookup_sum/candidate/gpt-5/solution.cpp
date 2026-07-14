#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) return 0ull;

  const uint32_t* s = starts.data();
  const uint32_t* e = ends.data();
  const uint32_t* pl = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;
  std::size_t i = 0; // interval index

  for (std::size_t k = 0; k < m; ++k) {
    const uint32_t p = pr[k];

    // Advance interval index while current interval ends before or at probe
    while (i < n && e[i] <= p) {
      ++i;
    }
    if (i == n) {
      // No more intervals can match any subsequent (larger) probes
      break;
    }

    // If probe is within [start, end), accumulate weighted payload
    if (s[i] <= p) {
      sum += static_cast<uint64_t>(pl[i]) * static_cast<uint64_t>(w[k]);
    }
    // else: probe lies before the current interval; no match for this probe
  }

  return sum;
}
