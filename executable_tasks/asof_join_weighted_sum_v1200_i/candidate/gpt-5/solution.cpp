#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t dn = dim_ts.size();
  const std::size_t pn = probe_ts.size();

  if (pn == 0) return 0;

  const uint32_t* dt = dim_ts.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  std::size_t j = 0;
  uint32_t last_val = 0;
  uint64_t sum = 0;

  for (std::size_t i = 0; i < pn; ++i) {
    const uint32_t t = pt[i];
    while (j < dn && dt[j] <= t) {
      last_val = dv[j];
      ++j;
    }
    sum += static_cast<uint64_t>(last_val) * static_cast<uint64_t>(pw[i]);
  }

  return sum;
}
