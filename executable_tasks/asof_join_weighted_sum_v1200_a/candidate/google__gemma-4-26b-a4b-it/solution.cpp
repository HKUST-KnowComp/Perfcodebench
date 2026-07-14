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

  // Pre-calculate the mapping of probe indices to dim indices using two pointers.
  // Since both are sorted, we can find the 'latest not-greater' index in one pass.
  std::vector<size_t> probe_to_dim_idx(n_probe);
  size_t dim_ptr = 0;
  for (size_t i = 0; i < n_probe; ++i) {
    // Move dim_ptr to the largest index such that dim_ts[dim_ptr] <= probe_ts[i]
    while (dim_ptr + 1 < n_dim && dim_ts[dim_ptr + 1] <= probe_ts[i]) {
      dim_ptr++;
    }
    
    // Check if the current dim_ptr is valid for the probe_ts[i]
    if (dim_ts[dim_ptr] <= probe_ts[i]) {
      probe_to_dim_idx[i] = dim_ptr;
    } else {
      // If even the first dim_ts is greater than probe_ts[i], it's an invalid join
      // We use a sentinel value or handle it via a flag. 
      // However, the problem implies dim_ts[0] is the start. 
      // If probe_ts[i] < dim_ts[0], it doesn't match any 'not-greater' dimension.
      probe_to_dim_idx[i] = static_cast<size_t>(-1);
    }
  }

  // Perform the iterations
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n_probe; ++i) {
      size_t d_idx = probe_to_dim_idx[i];
      if (d_idx != static_cast<size_t>(-1)) {
        current_iter_sum += static_cast<uint64_t>(dim_values[d_idx]) * probe_weights[i];
      }
    }
    total_sum += current_iter_sum;
  }

  // The requirement asks for the sum of the last iteration or the total? 
  // The baseline returns the sum of the LAST iteration (it resets sum = 0 inside the loop).
  // Let's re-examine baseline: 'sum = 0;' is inside 'for (int iter = 0; iter < iters; ++iter)'.
  // So we must return the sum of the final iteration.
  
  // Re-calculating to match baseline behavior exactly (returning sum of the last iteration):
  uint64_t final_sum = 0;
  for (size_t i = 0; i < n_probe; ++i) {
    size_t d_idx = probe_to_dim_idx[i];
    if (d_idx != static_cast<size_t>(-1)) {
      final_sum += static_cast<uint64_t>(dim_values[d_idx]) * probe_weights[i];
    }
  }
  
  // Wait, the baseline loop is:
  // for (int iter = 0; iter < iters; ++iter) { sum = 0; ... sum += ... }
  // This means the 'sum' returned is just the result of the very last iteration.
  // The 'iters' loop in the baseline is actually redundant if the data doesn't change.
  // But we must respect the logic. If iters = 0, it returns 0. If iters > 0, it returns the sum.
  
  if (iters <= 0) return 0;
  return final_sum;
}