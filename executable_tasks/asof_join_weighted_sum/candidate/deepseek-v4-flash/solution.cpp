#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters == 0) return 0;

  uint64_t sum = 0;
  std::size_t dim_idx = 0;
  const std::size_t n = dim_ts.size();
  const std::size_t m = probe_ts.size();

  for (std::size_t p = 0; p < m; ++p) {
    // Advance dim_idx while the next dimension timestamp is <= probe_ts[p]
    while (dim_idx + 1 < n && dim_ts[dim_idx + 1] <= probe_ts[p]) {
      ++dim_idx;
    }
    // dim_ts[dim_idx] is the latest not-greater, assumption: always exists (dim_ts[0] <= probe_ts[0])
    sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[p]);
  }

  return sum * static_cast<uint64_t>(iters);
}
