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

  // Pre-fetch pointers to avoid repeated vector overhead
  const uint32_t* __restrict dim_ts_ptr = dim_ts.data();
  const uint32_t* __restrict dim_val_ptr = dim_values.data();
  const uint32_t* __restrict probe_ts_ptr = probe_ts.data();
  const uint32_t* __restrict probe_w_ptr = probe_weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t dim_idx = 0;

    for (size_t i = 0; i < n_probe; ++i) {
      uint32_t current_probe_ts = probe_ts_ptr[i];

      // Advance dim_idx to the largest index such that dim_ts[dim_idx] <= current_probe_ts
      // Since probe_ts is sorted, dim_idx only moves forward.
      while (dim_idx + 1 < n_dim && dim_ts_ptr[dim_idx + 1] <= current_probe_ts) {
        dim_idx++;
      }

      // Check if the current probe_ts is at least as large as the first dimension timestamp
      if (dim_ts_ptr[dim_idx] <= current_probe_ts) {
        current_iter_sum += static_cast<uint64_t>(dim_val_ptr[dim_idx]) * probe_w_ptr[i];
      }
    }
    total_sum += current_iter_sum;
  }

  return total_sum;
}