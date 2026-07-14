#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) {
    return 0;
  }

  const uint32_t* s = starts.data();
  const uint32_t* e = ends.data();
  const uint32_t* p = probes.data();
  const uint32_t* w = weights.data();

  std::size_t i = 0; // index into probes/weights
  uint64_t sum = 0;

  for (std::size_t j = 0; j < n && i < m; ++j) {
    const uint32_t sj = s[j];
    const uint32_t ej = e[j];

    // Skip probes that are before the current interval start.
    while (i < m && p[i] < sj) {
      ++i;
    }
    if (i >= m) {
      break;
    }

    // Ignore empty or invalid intervals.
    if (sj >= ej) {
      continue;
    }

    // Accumulate weights for probes within [sj, ej).
    uint64_t wsum = 0;
    while (i < m) {
      const uint32_t val = p[i];
      if (val >= ej) {
        break;
      }
      // val >= sj holds due to the skip loop above
      wsum += static_cast<uint64_t>(w[i]);
      ++i;
    }

    if (wsum != 0) {
      sum += static_cast<uint64_t>(payloads[j]) * wsum;
    }
  }

  return sum;
}
