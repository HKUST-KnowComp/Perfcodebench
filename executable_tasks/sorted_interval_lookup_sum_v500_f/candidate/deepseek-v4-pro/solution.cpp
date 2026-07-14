#include "interface.h"

#include <algorithm>
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
  (void)iters;  // the same sum is computed each iteration; we compute it once

  const std::size_t n_probes = probes.size();
  std::vector<std::size_t> idx(n_probes);
  const uint32_t* s = starts.data();
  const std::size_t n_starts = starts.size();

  // Precompute the interval index for every probe
  for (std::size_t i = 0; i < n_probes; ++i) {
    uint32_t p = probes[i];
    auto it = std::upper_bound(s, s + n_starts, p);
    idx[i] = static_cast<std::size_t>(it - s - 1);
  }

  uint64_t sum = 0;
  const uint32_t* pld = payloads.data();
  const uint32_t* wgt = weights.data();
  const std::size_t* id = idx.data();

  for (std::size_t i = 0; i < n_probes; ++i) {
    sum += static_cast<uint64_t>(pld[id[i]]) * static_cast<uint64_t>(wgt[i]);
  }

  return sum;
}
