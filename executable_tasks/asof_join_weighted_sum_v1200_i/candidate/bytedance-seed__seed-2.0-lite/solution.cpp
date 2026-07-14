#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const std::size_t dim_size = dim_ts.size();
  const std::size_t probe_count = probe_ts.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    for (std::size_t i = 0; i < probe_count; ++i) {
      const uint32_t current_probe_ts = probe_ts[i];
      while (dim_idx < dim_size - 1 && dim_ts[dim_idx + 1] <= current_probe_ts) {
        dim_idx++;
      }
      sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}