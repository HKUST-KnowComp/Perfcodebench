#include "interface.h"

#include <algorithm>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  
  // Two-pointer approach for sorted arrays - O(M + N) instead of O(M * log N)
  std::size_t dim_idx = 0;
  const std::size_t dim_size = dim_ts.size();
  
  for (std::size_t i = 0; i < probe_ts.size(); ++i) {
    // Advance dim_idx while dim_ts[dim_idx] <= probe_ts[i]
    while (dim_idx < dim_size && dim_ts[dim_idx] <= probe_ts[i]) {
      ++dim_idx;
    }
    
    // dim_idx is now the first element > probe_ts[i], so dim_idx - 1 is the latest <= probe_ts[i]
    if (dim_idx > 0) {
      sum += static_cast<uint64_t>(dim_values[dim_idx - 1]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  
  // Multiply by iters instead of repeating the entire join loop
  return sum * static_cast<uint64_t>(iters);
}