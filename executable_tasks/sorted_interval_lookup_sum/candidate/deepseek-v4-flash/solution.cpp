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
  
  const std::size_t n = probes.size();
  const std::size_t m = starts.size();
  const uint32_t* const s = starts.data();
  const uint32_t* const p = payloads.data();
  const uint32_t* const pr = probes.data();
  const uint32_t* const w = weights.data();
  
  uint64_t sum = 0;
  std::size_t idx = 0;  // index of current best interval
  
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t probe = pr[i];
    // advance while the next interval start is <= probe
    while (idx + 1 < m && s[idx + 1] <= probe) {
      ++idx;
    }
    sum += static_cast<uint64_t>(p[idx]) * static_cast<uint64_t>(w[i]);
  }
  
  return sum;
}
