#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  // Assumes dim_ts and probe_ts are sorted ascending (per task description).
  const std::size_t m = dim_ts.size();
  const std::size_t n = probe_ts.size();

  const uint32_t* dt = dim_ts.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  uint64_t sum = 0;
  std::size_t j = 0;            // index into dim_ts/dim_values
  uint32_t last_val = 0;        // last matched dim value (for current probe)

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t p = pt[i];
    while (j < m && dt[j] <= p) {
      last_val = dv[j];
      ++j;
    }
    sum += static_cast<uint64_t>(last_val) * static_cast<uint64_t>(pw[i]);
  }

  return sum;
}
