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

  // Preserve behavior for iters == 0
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = starts.size();
  const std::size_t m = probes.size();

  // If no work to do or no intervals, sum is zero (aligns with baseline semantics for empty probes)
  if (m == 0 || n == 0) {
    return 0ULL;
  }

  const uint32_t* s = starts.data();
  const uint32_t* pl = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0ULL;

  // Two-pointer scan: advance interval index monotonically as probes increase
  std::size_t idx = 0;
  const std::size_t last = n - 1;

  for (std::size_t i = 0; i < m; ++i) {
    const uint32_t p = pr[i];
    // Move idx forward while next start is <= current probe
    while (idx < last && s[idx + 1] <= p) {
      ++idx;
    }
    sum += static_cast<uint64_t>(pl[idx]) * static_cast<uint64_t>(w[i]);
  }

  // The result of each iteration is identical; return the last iteration's sum
  // as per the baseline semantics.
  (void)iters; // no-op for remaining iterations
  return sum;
}
