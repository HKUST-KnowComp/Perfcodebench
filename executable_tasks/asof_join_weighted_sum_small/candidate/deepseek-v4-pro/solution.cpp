#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0 || probe_ts.empty()) return 0;
  uint64_t sum = 0;
  std::size_t j = 0;
  for (std::size_t i = 0; i < probe_ts.size(); ++i) {
    while (j + 1 < dim_ts.size() && dim_ts[j + 1] <= probe_ts[i]) {
      ++j;
    }
    sum += static_cast<uint64_t>(dim_values[j]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}