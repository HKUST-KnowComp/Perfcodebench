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

  if (iters <= 0 || probe_n == 0 || dim_n == 0) {
    return 0;
  }

  const uint32_t* const dim_ts_data = dim_ts.data();
  const uint32_t* const dim_val_data = dim_values.data();
  const uint32_t* const probe_ts_data = probe_ts.data();
  const uint32_t* const probe_w_data = probe_weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t j = 0;

    for (std::size_t i = 0; i < probe_n; ++i) {
      const uint32_t pts = probe_ts_data[i];

      while (j + 1 < dim_n && dim_ts_data[j + 1] <= pts) {
        ++j;
      }

      local_sum += static_cast<uint64_t>(dim_val_data[j]) *
                   static_cast<uint64_t>(probe_w_data[i]);
    }

    sum = local_sum;
  }

  return sum;
}
