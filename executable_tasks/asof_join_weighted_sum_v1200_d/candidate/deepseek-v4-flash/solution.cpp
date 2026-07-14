#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const size_t dim_len = dim_ts.size();
  const size_t probe_len = probe_ts.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t j = 0;
    for (size_t i = 0; i < probe_len; ++i) {
      uint32_t pt = probe_ts[i];
      while (j < dim_len && dim_ts[j] <= pt) {
        ++j;
      }
      // j is at least 1 because there is a dim_ts <= probe_ts[i]
      sum += static_cast<uint64_t>(dim_values[j-1]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}