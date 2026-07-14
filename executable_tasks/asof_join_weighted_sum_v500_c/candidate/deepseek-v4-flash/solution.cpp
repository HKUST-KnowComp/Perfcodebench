#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t single_sum = 0;
  const std::size_t dim_size = dim_ts.size();
  const std::size_t probe_size = probe_ts.size();
  if (dim_size == 0 || probe_size == 0) return 0;

  const uint32_t* dim_ts_ptr = dim_ts.data();
  const uint32_t* dim_val_ptr = dim_values.data();
  const uint32_t* probe_ts_ptr = probe_ts.data();
  const uint32_t* probe_w_ptr = probe_weights.data();

  int64_t j = -1; // index of last dim_ts <= current probe_ts
  for (std::size_t i = 0; i < probe_size; ++i) {
    while (j + 1 < static_cast<int64_t>(dim_size) && dim_ts_ptr[j + 1] <= probe_ts_ptr[i]) {
      ++j;
    }
    uint32_t val = (j >= 0) ? dim_val_ptr[j] : 0;
    single_sum += static_cast<uint64_t>(val) * static_cast<uint64_t>(probe_w_ptr[i]);
  }
  return single_sum * static_cast<uint64_t>(iters);
}