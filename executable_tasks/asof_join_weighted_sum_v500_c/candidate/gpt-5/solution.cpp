#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t dn = dim_ts.size();
  const std::size_t pn = probe_ts.size();
  if (dn == 0 || pn == 0) return 0;

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dvs = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pws = probe_weights.data();

  uint64_t sum = 0;

  // Skip probes that are earlier than the first dimension timestamp (no match).
  std::size_t i = 0;
  const uint32_t first_ts = dts[0];
  while (i < pn && pts[i] < first_ts) ++i;

  // Two-pointer linear as-of join.
  std::size_t j = 0;  // index into dim_ts/dim_values
  for (; i < pn; ++i) {
    const uint32_t p = pts[i];
    while ((j + 1) < dn && dts[j + 1] <= p) ++j;
    sum += static_cast<uint64_t>(dvs[j]) * static_cast<uint64_t>(pws[i]);
  }

  return sum;
}
