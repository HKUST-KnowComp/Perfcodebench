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
  const std::size_t nStarts = starts.size();
  const std::size_t nProbes = probes.size();
  if (nStarts == 0 || nProbes == 0) return 0;

  const uint32_t* s = starts.data();
  const uint32_t* e = ends.data();
  const uint32_t* pl = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;
  std::size_t i = 0; // index into intervals (starts/ends/payloads)

  for (std::size_t j = 0; j < nProbes; ++j) {
    const uint32_t p = pr[j];
    while (i < nStarts && s[i] <= p) {
      ++i;
    }
    if (i == 0) {
      // Probe is before the first interval start; no match.
      continue;
    }
    const std::size_t idx = i - 1;
    if (p <= e[idx]) {
      sum += static_cast<uint64_t>(pl[idx]) * static_cast<uint64_t>(w[j]);
    }
    // else: probe lies beyond the end of idx; no match.
  }

  return sum;
}
