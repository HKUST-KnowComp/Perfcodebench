#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t dsz = dim_ts.size();
  const std::size_t psz = probe_ts.size();
  if (dsz == 0 || psz == 0) return 0ULL;

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dvals = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pws = probe_weights.data();

  uint64_t sum = 0ULL;
  std::size_t j = 0;
  uint64_t currentValue = 0ULL;

  for (std::size_t i = 0; i < psz; ++i) {
    const uint32_t t = pts[i];
    while (j < dsz && dts[j] <= t) {
      currentValue = static_cast<uint64_t>(dvals[j]);
      ++j;
    }
    sum += currentValue * static_cast<uint64_t>(pws[i]);
  }

  return sum;
}
