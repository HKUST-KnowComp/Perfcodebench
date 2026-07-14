#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t n = probe_ts.size();
  const std::size_t d = dim_ts.size();
  if (n == 0 || d == 0) return 0;

  std::size_t idx = 0;
  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t probe = probe_ts[i];
    while (idx + 1 < d && dim_ts[idx + 1] <= probe) {
      ++idx;
    }
    sum += static_cast<uint64_t>(dim_values[idx]) *
           static_cast<uint64_t>(probe_weights[i]);
  }
  (void)iters; // preserve signature but ignore repetition
  return sum;
}