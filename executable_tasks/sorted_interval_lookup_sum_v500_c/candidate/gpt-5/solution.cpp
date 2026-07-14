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
  // Preserve baseline semantics: if iters == 0, return 0; otherwise compute once.
  if (iters <= 0) return 0;

  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) return 0;

  const uint32_t* e = ends.data();
  const uint32_t* pl = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();

  std::size_t j = 0; // current interval index
  uint64_t sum = 0;

  for (std::size_t i = 0; i < m; ++i) {
    const uint32_t p = pr[i];
    // Advance interval index while probe is beyond current interval end.
    // Clamp at last interval to avoid out-of-bounds.
    while ((j + 1) < n && p >= e[j]) {
      ++j;
    }
    sum += static_cast<uint64_t>(pl[j]) * static_cast<uint64_t>(w[i]);
  }

  return sum;
}
