#include "interface.h"

#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  // Compute once; repeated iterations would yield the same sum.
  const std::size_t n = dim_ts.size();
  const std::size_t m = probe_ts.size();
  if (n == 0 || m == 0) return 0;

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dvals = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pws = probe_weights.data();

  uint64_t sum = 0;
  std::size_t j = 0; // index into dim arrays; j is the last dim_ts index with ts <= current probe

  for (std::size_t i = 0; i < m; ++i) {
    const uint32_t p = pts[i];
    // advance j while next dim timestamp is still <= probe timestamp
    while ((j + 1) < n && dts[j + 1] <= p) {
      ++j;
    }
    // Only add if there is a matching dim timestamp <= probe timestamp
    if (dts[j] <= p) {
      sum += static_cast<uint64_t>(dvals[j]) * static_cast<uint64_t>(pws[i]);
    }
  }

  return sum;
}
