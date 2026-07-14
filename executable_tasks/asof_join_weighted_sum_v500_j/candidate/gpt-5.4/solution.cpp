#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t dim_n = dim_ts.size();
  const std::size_t probe_n = probe_ts.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t j = 0;
    uint32_t current_value = dim_values[0];

    for (std::size_t i = 0; i < probe_n; ++i) {
      const uint32_t ts = probe_ts[i];

      while (j + 1 < dim_n && dim_ts[j + 1] <= ts) {
        ++j;
        current_value = dim_values[j];
      }

      local_sum += static_cast<uint64_t>(current_value) *
                   static_cast<uint64_t>(probe_weights[i]);
    }

    sum = local_sum;
  }

  return sum;
}
