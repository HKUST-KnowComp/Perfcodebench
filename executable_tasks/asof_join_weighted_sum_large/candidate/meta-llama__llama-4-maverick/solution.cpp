#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const auto dim_ts_size = dim_ts.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_ts.size(); ++i) {
      std::size_t low = 0;
      std::size_t high = dim_ts_size;
      while (low < high) {
        const auto mid = (low + high) / 2;
        if (dim_ts[mid] <= probe_ts[i]) {
          low = mid + 1;
        } else {
          high = mid;
        }
      }
      const std::size_t idx = low - 1;
      sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}