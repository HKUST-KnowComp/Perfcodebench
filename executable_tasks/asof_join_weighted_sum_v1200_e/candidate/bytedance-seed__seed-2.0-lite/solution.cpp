#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t single_sum = 0;
  std::size_t count = 0;
  const std::size_t dim_size = dim_ts.size();
  const std::size_t probe_size = probe_ts.size();
  for (std::size_t i = 0; i < probe_size; ++i) {
    const uint32_t p_ts = probe_ts[i];
    while (count < dim_size && dim_ts[count] <= p_ts) {
      ++count;
    }
    const std::size_t idx = count - 1;
    single_sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return single_sum * static_cast<uint64_t>(iters);
}