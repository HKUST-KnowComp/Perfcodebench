#include "interface.h"

#include <cstddef>
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

  const uint32_t* s = starts.data();
  const uint32_t* e = ends.data();
  const uint32_t* pl = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;

  std::size_t i = 0; // interval index
  for (std::size_t j = 0; j < m; ++j) {
    const uint32_t probe = pr[j];

    // Advance intervals until one could contain probe or intervals are exhausted
    while (i < n && e[i] < probe) {
      ++i;
    }
    if (i == n) {
      break; // no more intervals can match any remaining (sorted) probes
    }

    // If probe is before the start of the current interval, it does not match
    if (probe < s[i]) {
      continue;
    }

    // probe is within [s[i], e[i]]
    sum += static_cast<uint64_t>(pl[i]) * static_cast<uint64_t>(w[j]);
  }

  return sum;
}
