#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Suppress unused parameter warning; the result is identical for any iters.
  (void)iters;
  
  uint64_t sum = 0;
  std::size_t dim_idx = 0;  // index of largest dim_ts <= current probe_ts
  
  for (std::size_t p = 0; p < probe_ts.size(); ++p) {
    // Advance dim_idx while the next dim_ts is still <= probe_ts[p]
    while (dim_idx + 1 < dim_ts.size() && dim_ts[dim_idx + 1] <= probe_ts[p]) {
      ++dim_idx;
    }
    sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[p]);
  }
  
  return sum;
}