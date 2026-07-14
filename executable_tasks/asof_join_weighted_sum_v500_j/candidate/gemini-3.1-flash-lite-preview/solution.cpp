#include "interface.h"

#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = dim_ts.size();
  const size_t m = probe_ts.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t dim_idx = 0;
    
    for (size_t i = 0; i < m; ++i) {
      // Advance dim_idx to the last element <= probe_ts[i]
      while (dim_idx + 1 < n && dim_ts[dim_idx + 1] <= probe_ts[i]) {
        dim_idx++;
      }
      
      // Check if a valid match exists (dim_ts[dim_idx] <= probe_ts[i])
      if (dim_ts[dim_idx] <= probe_ts[i]) {
        current_iter_sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}