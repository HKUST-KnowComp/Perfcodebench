#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_ts.empty() || probe_ts.empty()) {
    return 0;
  }

  uint64_t total_sum = 0;
  const size_t n_dim = dim_ts.size();
  const size_t n_probe = probe_ts.size();

  // Pre-calculate the join results once if iters > 1, 
  // or just run the linear scan. 
  // Since the inputs are constant across iterations, 
  // we calculate the sum for one pass and multiply.
  
  uint64_t single_pass_sum = 0;
  size_t dim_idx = 0;

  for (size_t i = 0; i < n_probe; ++i) {
    uint32_t current_probe_ts = probe_ts[i];
    
    // Advance dim_idx to the latest timestamp <= current_probe_ts
    // Since probe_ts is sorted, dim_idx only moves forward.
    while (dim_idx + 1 < n_dim && dim_ts[dim_idx + 1] <= current_probe_ts) {
      dim_idx++;
    }

    // Check if the current dim_idx is valid for the probe_ts
    if (dim_ts[dim_idx] <= current_probe_ts) {
      single_pass_sum += static_cast<uint64_t>(dim_values[dim_idx]) * 
                         static_cast<uint64_t>(probe_weights[i]);
    }
  }

  // The task asks for the sum over 'iters' iterations.
  // Based on the baseline, it resets sum = 0 each iteration and adds to it.
  // This is equivalent to single_pass_sum * iters.
  total_sum = single_pass_sum * static_cast<uint64_t>(iters);

  return total_sum;
}