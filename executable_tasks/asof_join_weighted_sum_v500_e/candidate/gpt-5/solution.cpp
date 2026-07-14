#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t dsz = dim_ts.size();
  const std::size_t psz = probe_ts.size();
  if (dsz == 0 || psz == 0) return 0;

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dvals = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pws = probe_weights.data();

  std::size_t j = 0;
  uint64_t base_sum = 0;

  for (std::size_t i = 0; i < psz; ++i) {
    const uint32_t p = pts[i];
    while ((j + 1) < dsz && dts[j + 1] <= p) {
      ++j;
    }
    if (dts[j] <= p) {
      base_sum += static_cast<uint64_t>(dvals[j]) * static_cast<uint64_t>(pws[i]);
    }
  }

  return base_sum * static_cast<uint64_t>(iters);
}
