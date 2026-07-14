#include "interface.h"
#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  std::size_t n = dim_ts.size();
  std::size_t m = probe_ts.size();
  if (n == 0 || m == 0) return 0;
  uint64_t sum = 0;
  std::size_t j = 0;
  for (std::size_t i = 0; i < m; ++i) {
    while (j + 1 < n && dim_ts[j + 1] <= probe_ts[i]) {
      ++j;
    }
    sum += static_cast<uint64_t>(dim_values[j]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}
