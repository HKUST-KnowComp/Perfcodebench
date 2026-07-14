#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0) return 0;
  const std::size_t n = probe_ts.size();
  if (n == 0) return 0;

  // Precompute the index of the latest dimension timestamp not greater than each probe.
  // Both arrays are sorted, so a single linear scan suffices.
  std::vector<std::size_t> idx(n);
  std::size_t d = 0;
  const std::size_t dim_sz = dim_ts.size();
  for (std::size_t i = 0; i < n; ++i) {
    while (d < dim_sz && dim_ts[d] <= probe_ts[i]) {
      ++d;
    }
    idx[i] = d - 1;  // valid because probe_ts[i] >= dim_ts[0] by problem precondition
  }

  // Compute the weighted sum once (equivalent to last iteration of original loop).
  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(dim_values[idx[i]]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}