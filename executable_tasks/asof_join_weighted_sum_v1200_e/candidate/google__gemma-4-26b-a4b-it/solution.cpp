#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n_dim = dim_ts.size();
  const size_t n_probe = probe_ts.size();

  if (n_dim == 0 || n_probe == 0) {
    return 0;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t dim_idx = 0;

    for (size_t i = 0; i < n_probe; ++i) {
      uint32_t target = probe_ts[i];
      
      // Advance dim_idx to the largest index such that dim_ts[dim_idx] <= target
      // Since probe_ts is sorted, dim_idx only moves forward.
      while (dim_idx + 1 < n_dim && dim_ts[dim_idx + 1] <= target) {
        dim_idx++;
      }

      // Check if the current dim_idx is valid for the as-of join
      if (dim_ts[dim_idx] <= target) {
        current_iter_sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}