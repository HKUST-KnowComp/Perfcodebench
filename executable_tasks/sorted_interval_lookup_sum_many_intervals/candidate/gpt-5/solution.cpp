#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) return 0ULL;

  const uint32_t* s = starts.data();
  const uint32_t* e = ends.data();
  const uint32_t* pay = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0ULL;
  std::size_t j = 0; // interval index

  for (std::size_t i = 0; i < m; ++i) {
    const uint32_t p = pr[i];

    // Advance interval index while current interval ends before probe
    while (j < n && e[j] < p) {
      ++j;
    }
    if (j == n) {
      // No more intervals can match subsequent (sorted, increasing) probes
      break;
    }

    // Check if probe falls within the current interval
    if (p >= s[j]) {
      const uint32_t wi = w[i];
      if (wi != 0) {
        sum += static_cast<uint64_t>(pay[j]) * static_cast<uint64_t>(wi);
      }
    }
    // else: probe is before the start of current interval -> no match
  }

  return sum;
}
