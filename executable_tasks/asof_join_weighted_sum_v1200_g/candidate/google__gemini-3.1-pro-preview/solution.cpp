#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  if (probe_ts.empty()) return 0;
  
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();
  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_val = dim_values.data();
  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_w = probe_weights.data();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t dim_idx = 0;

    // Two-pointer approach: O(P + D) complexity
    // Since both dim_ts and probe_ts are sorted, we can advance dim_idx monotonically.
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t pts = p_ts[i];
      
      // Advance dim_idx to the latest timestamp not greater than probe_ts[i]
      // We check dim_idx + 1 to see if the next dimension entry is still valid for the current probe.
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts) {
        dim_idx++;
      }

      // The baseline implementation uses (upper_bound - 1), which corresponds to the 
      // largest index j such that dim_ts[j] <= probe_ts[i].
      current_sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
    }
    total_sum = current_sum;
  }

  return total_sum;
}