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

  const std::size_t dim_n = dim_ts.size();
  const std::size_t probe_n = probe_ts.size();

  const uint32_t* dim_ts_data = dim_ts.data();
  const uint32_t* dim_val_data = dim_values.data();
  const uint32_t* probe_ts_data = probe_ts.data();
  const uint32_t* probe_w_data = probe_weights.data();

  uint64_t sum = 0;

  std::size_t j = 0;
  uint32_t current_value = dim_val_data[0];

  for (std::size_t i = 0; i < probe_n; ++i) {
    const uint32_t pt = probe_ts_data[i];

    while (j + 1 < dim_n && dim_ts_data[j + 1] <= pt) {
      ++j;
      current_value = dim_val_data[j];
    }

    sum += static_cast<uint64_t>(current_value) *
           static_cast<uint64_t>(probe_w_data[i]);
  }

  return sum;
}
