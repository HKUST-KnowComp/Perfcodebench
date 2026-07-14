#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    auto dim_it = dim_ts.begin();
    for (std::size_t i = 0; i < probe_ts.size(); ++i) {
      dim_it = std::upper_bound(dim_it, dim_ts.end(), probe_ts[i]);
      const std::size_t idx = static_cast<std::size_t>(dim_it - dim_ts.begin() - 1);
      sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}