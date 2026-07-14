#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t sum = 0;
  const std::size_t probe_count = probe_ts.size();
  if (probe_count == 0) return 0;
  
  std::size_t dim_idx = 0;
  const std::size_t dim_size = dim_ts.size();
  
  for (std::size_t i = 0; i < probe_count; ++i) {
    // Advance dim_idx to the last dim_ts <= probe_ts[i]
    while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= probe_ts[i]) {
      ++dim_idx;
    }
    sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}