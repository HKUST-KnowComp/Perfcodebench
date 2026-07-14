#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t dim_size = dim_ts.size();
  const std::size_t probe_size = probe_ts.size();
  uint64_t final_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    std::size_t dim_ptr = 0;
    for (std::size_t i = 0; i < probe_size; ++i) {
      const uint32_t curr_probe = probe_ts[i];
      while (dim_ptr + 1 < dim_size && dim_ts[dim_ptr + 1] <= curr_probe) {
        dim_ptr++;
      }
      sum += static_cast<uint64_t>(dim_values[dim_ptr]) * static_cast<uint64_t>(probe_weights[i]);
    }
    final_sum = sum;
  }
  return final_sum;
}