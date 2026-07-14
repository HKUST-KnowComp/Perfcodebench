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

  const std::size_t n_probes = probes.size();
  const std::size_t n_starts = starts.size();

  if (n_probes == 0 || n_starts == 0) {
    return 0ull;
  }

  const uint32_t* s = starts.data();
  const uint32_t* pld = payloads.data();
  const uint32_t* prb = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;

  // Maintain hi as the count of starts <= current probe (upper_bound index)
  std::size_t hi = 0;
  for (std::size_t i = 0; i < n_probes; ++i) {
    const uint32_t x = prb[i];
    while (hi < n_starts && s[hi] <= x) {
      ++hi;
    }
    // Assumes inputs guarantee at least one start <= probe.
    sum += static_cast<uint64_t>(pld[hi - 1]) * static_cast<uint64_t>(w[i]);
  }

  return sum;
}
