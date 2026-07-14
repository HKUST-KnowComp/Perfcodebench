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

  if (iters <= 0) return 0;

  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) return 0;

  const uint32_t* s = starts.data();
  const uint32_t* plds = payloads.data();
  const uint32_t* ps = probes.data();
  const uint32_t* ws = weights.data();

  std::size_t idx = 0;
  uint64_t sum = 0;

  // Preload next start value for faster comparisons
  uint32_t next = (n > 1) ? s[1] : 0xFFFFFFFFu;

  for (std::size_t i = 0; i < m; ++i) {
    const uint32_t pv = ps[i];
    // Advance interval index while the next interval start is <= probe
    while ((idx + 1) < n && next <= pv) {
      ++idx;
      next = ((idx + 1) < n) ? s[idx + 1] : 0xFFFFFFFFu;
    }
    sum += static_cast<uint64_t>(plds[idx]) * static_cast<uint64_t>(ws[i]);
  }

  return sum;
}
