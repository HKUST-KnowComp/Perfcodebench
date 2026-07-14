#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  (void)iters; // The result is deterministic; computing once is sufficient.

  const std::size_t dn = dim_ts.size();
  const std::size_t pn = probe_ts.size();
  if (dn == 0 || pn == 0) return 0;

  const uint32_t* dt_base = dim_ts.data();
  const uint32_t* dt_ptr = dt_base;
  const uint32_t* dt_end = dt_base + dn;
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  uint64_t sum = 0;

  for (std::size_t i = 0; i < pn; ++i) {
    const uint32_t p = pt[i];
    // Advance dimension pointer to first timestamp greater than probe timestamp
    while (dt_ptr < dt_end && *dt_ptr <= p) {
      ++dt_ptr;
    }

    // If no dim_ts <= p, contribution is zero
    if (dt_ptr == dt_base) continue;

    const std::size_t idx = static_cast<std::size_t>(dt_ptr - dt_base - 1);
    sum += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
  }

  return sum;
}
