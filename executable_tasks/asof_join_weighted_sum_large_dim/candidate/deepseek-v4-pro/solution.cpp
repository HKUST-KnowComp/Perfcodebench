#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t n_probes = probe_ts.size();
  const std::size_t n_dims = dim_ts.size();
  if (n_probes == 0 || n_dims == 0) return 0;

  std::size_t j = 0;
  uint64_t single_sum = 0;
  for (std::size_t i = 0; i < n_probes; ++i) {
    uint32_t pt = probe_ts[i];
    while (j + 1 < n_dims && dim_ts[j + 1] <= pt) {
      ++j;
    }
    single_sum += static_cast<uint64_t>(dim_values[j]) * static_cast<uint64_t>(probe_weights[i]);
  }
  uint64_t total = single_sum * static_cast<uint64_t>(iters);
  return total;
}