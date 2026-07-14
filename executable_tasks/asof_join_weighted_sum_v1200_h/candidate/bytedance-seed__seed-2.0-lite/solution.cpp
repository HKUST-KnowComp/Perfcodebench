#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();
  uint64_t final_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t dim_idx = 0;

    for (size_t i = 0; i < probe_size; ++i) {
      const uint32_t current_probe_ts = probe_ts[i];
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= current_probe_ts) {
        dim_idx++;
      }
      sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    final_sum = sum;
  }
  return final_sum;
}