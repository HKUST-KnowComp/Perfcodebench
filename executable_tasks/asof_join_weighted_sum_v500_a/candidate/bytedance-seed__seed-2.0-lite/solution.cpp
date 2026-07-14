#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_ts.empty() || probe_ts.empty()) return 0;
  
  uint64_t sum = 0;
  std::size_t dim_idx = 0;
  const std::size_t dim_size = dim_ts.size();
  
  for (std::size_t i = 0; i < probe_ts.size(); ++i) {
    const uint32_t current_probe = probe_ts[i];
    while (dim_idx < dim_size && dim_ts[dim_idx] <= current_probe) {
      ++dim_idx;
    }
    const std::size_t idx = dim_idx - 1;
    sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
  }
  
  return sum;
}