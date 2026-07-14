#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const std::size_t dim_size = dim_ts.size();
  const std::size_t probe_size = probe_ts.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t j = 0;
    for (std::size_t i = 0; i < probe_size; ++i) {
      const uint32_t current_probe = probe_ts[i];
      while (j + 1 < dim_size && dim_ts[j + 1] <= current_probe) {
        j++;
      }
      sum += static_cast<uint64_t>(dim_values[j]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}