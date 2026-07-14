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

  const uint32_t* dim_ts_data = dim_n ? dim_ts.data() : nullptr;
  const uint32_t* dim_val_data = dim_n ? dim_values.data() : nullptr;
  const uint32_t* probe_ts_data = probe_n ? probe_ts.data() : nullptr;
  const uint32_t* probe_w_data = probe_n ? probe_weights.data() : nullptr;

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;

    std::size_t j = 0;
    uint32_t current_value = dim_val_data[0];

    for (std::size_t i = 0; i < probe_n; ++i) {
      const uint32_t pts = probe_ts_data[i];

      while (j + 1 < dim_n && dim_ts_data[j + 1] <= pts) {
        ++j;
        current_value = dim_val_data[j];
      }

      sum += static_cast<uint64_t>(current_value) *
             static_cast<uint64_t>(probe_w_data[i]);
    }
  }

  return sum;
}
