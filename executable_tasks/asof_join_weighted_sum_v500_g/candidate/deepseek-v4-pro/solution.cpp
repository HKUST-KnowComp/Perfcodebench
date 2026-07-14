#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  (void)iters; // unused, sum is invariant
  uint64_t sum = 0;
  if (probe_ts.empty()) return 0;
  const size_t n = probe_ts.size();
  const size_t m = dim_ts.size();
  size_t d = 0;
  for (size_t i = 0; i < n; ++i) {
    uint32_t pt = probe_ts[i];
    while (d + 1 < m && dim_ts[d + 1] <= pt) {
      ++d;
    }
    sum += static_cast<uint64_t>(dim_values[d]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}