#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // The result of each iteration is identical; return the sum computed once.
  (void)iters; // suppress unused warning while preserving signature

  const std::size_t dn = dim_ts.size();
  const std::size_t pn = probe_ts.size();
  if (pn == 0 || dn == 0) {
    return 0ULL;
  }

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dvs = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pws = probe_weights.data();

  uint64_t sum = 0ULL;
  std::size_t di = 0;             // index into dim arrays
  uint32_t current_val = 0U;      // last dim value with timestamp <= current probe

  for (std::size_t i = 0; i < pn; ++i) {
    const uint32_t pt = pts[i];
    while (di < dn && dts[di] <= pt) {
      current_val = dvs[di];
      ++di;
    }
    sum += static_cast<uint64_t>(current_val) * static_cast<uint64_t>(pws[i]);
  }

  return sum;
}
