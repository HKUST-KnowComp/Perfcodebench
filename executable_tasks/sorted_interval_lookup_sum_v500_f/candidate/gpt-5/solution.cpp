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
  (void)ends;
  (void)iters;

  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) return 0;

  const uint32_t* s = starts.data();
  const uint32_t* pay = payloads.data();
  const uint32_t* p = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;
  std::size_t pi = 0;

  // Process all intervals except the last, grouping probes by the next start boundary.
  if (n > 1) {
    for (std::size_t k = 0; k + 1 < n; ++k) {
      const uint32_t nextStart = s[k + 1];
      uint64_t wsum = 0;
      while (pi < m && p[pi] < nextStart) {
        wsum += static_cast<uint64_t>(w[pi]);
        ++pi;
      }
      sum += static_cast<uint64_t>(pay[k]) * wsum;
    }
  }

  // Remaining probes belong to the last interval.
  if (pi < m) {
    uint64_t wsum = 0;
    do {
      wsum += static_cast<uint64_t>(w[pi]);
      ++pi;
    } while (pi < m);
    sum += static_cast<uint64_t>(pay[n - 1]) * wsum;
  }

  return sum;
}
