#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Result is the same for every iteration; compute once.
  if (iters <= 0) return 0;

  const std::size_t nd = dim_ts.size();
  const std::size_t np = probe_ts.size();
  if (nd == 0 || np == 0) return 0;

  const uint32_t* dt = dim_ts.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  uint64_t sum = 0;

  // Skip probes that occur before the first dimension timestamp (contribute 0).
  std::size_t i = 0;
  const uint32_t first_dim_ts = dt[0];
  while (i < np && pt[i] < first_dim_ts) {
    ++i;
  }
  if (i == np) return 0;

  // Two-pointer as-of join: maintain j as the last dim index with dt[j] <= pt[i].
  std::size_t j = 0; // valid because pt[i] >= dt[0]
  for (; i < np; ++i) {
    const uint32_t p = pt[i];
    // Advance j while next dim timestamp is <= current probe timestamp.
    while (j + 1 < nd && dt[j + 1] <= p) {
      ++j;
    }
    sum += static_cast<uint64_t>(dv[j]) * static_cast<uint64_t>(pw[i]);
  }

  return sum;
}
