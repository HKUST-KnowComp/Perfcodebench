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
  
  if (probe_ts.empty() || dim_ts.empty()) {
    return 0;
  }

  const size_t num_probes = probe_ts.size();
  const size_t num_dims = dim_ts.size();
  const uint32_t* __restrict__ d_ts = dim_ts.data();
  const uint32_t* __restrict__ d_vals = dim_values.data();
  const uint32_t* __restrict__ p_ts = probe_ts.data();
  const uint32_t* __restrict__ p_ws = probe_weights.data();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t dim_idx = 0;

    // Since probe_ts is sorted, we can use a two-pointer approach.
    // For each probe_ts[i], we find the largest dim_ts[dim_idx] <= probe_ts[i].
    for (size_t i = 0; i < num_probes; ++i) {
      const uint32_t pts = p_ts[i];
      
      // Advance dim_idx to the latest timestamp that is not greater than the probe timestamp.
      // This assumes dim_ts is sorted.
      while (dim_idx + 1 < num_dims && d_ts[dim_idx + 1] <= pts) {
        dim_idx++;
      }

      // The baseline uses (upper_bound - 1), which corresponds to the largest index j 
      // such that dim_ts[j] <= probe_ts[i].
      current_sum += static_cast<uint64_t>(d_vals[dim_idx]) * static_cast<uint64_t>(p_ws[i]);
    }
    total_sum = current_sum;
  }

  return total_sum;
}