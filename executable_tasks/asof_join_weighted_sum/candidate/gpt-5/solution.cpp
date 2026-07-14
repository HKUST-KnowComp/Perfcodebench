#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t nd = dim_ts.size();
  const std::size_t np = probe_ts.size();
  if (nd == 0 || np == 0) return 0;

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dvals = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pws = probe_weights.data();

  uint64_t sum = 0;
  std::size_t j = 0; // index into dim_ts/values for current as-of match

  const uint32_t first_dim_ts = dts[0];

  for (std::size_t i = 0; i < np; ++i) {
    const uint32_t p = pts[i];
    if (p < first_dim_ts) {
      // No matching dim row; contribute 0
      continue;
    }
    // Advance j while next dim timestamp is still <= probe timestamp
    // Ensures dim_ts[j] is the last not-greater-than p.
    while ((j + 1) < nd && dts[j + 1] <= p) {
      ++j;
    }
    sum += static_cast<uint64_t>(dvals[j]) * static_cast<uint64_t>(pws[i]);
  }

  return sum;
}
