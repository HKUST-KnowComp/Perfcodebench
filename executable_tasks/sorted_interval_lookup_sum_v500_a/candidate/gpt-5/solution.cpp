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
  (void)iters;

  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) {
    return 0ULL;
  }

  const uint32_t* s = starts.data();
  const uint32_t* e = ends.data();
  const uint32_t* pay = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0ULL;
  std::size_t i = 0; // interval index

  for (std::size_t j = 0; j < m; ++j) {
    const uint32_t p = pr[j];

    // Advance intervals until the current interval's end is >= probe
    while (i < n && e[i] < p) {
      ++i;
    }
    if (i == n) {
      break; // no more intervals can match further probes
    }

    // If probe is within current interval bounds, accumulate
    if (p >= s[i]) {
      sum += static_cast<uint64_t>(pay[i]) * static_cast<uint64_t>(w[j]);
    }
    // else: probe < s[i], not covered by any interval; continue to next probe
  }

  return sum;
}
