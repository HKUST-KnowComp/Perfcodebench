#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

using std::size_t;
using std::uint32_t;
using std::uint64_t;

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const size_t dim_n  = dim_ts.size();
  const size_t probe_n = probe_ts.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t dim_idx = 0;
    for (size_t p = 0; p < probe_n; ++p) {
      const uint32_t ts = probe_ts[p];
      while (dim_idx + 1 < dim_n && dim_ts[dim_idx + 1] <= ts) {
        ++dim_idx;
      }
      sum += static_cast<uint64_t>(dim_values[dim_idx]) *
             static_cast<uint64_t>(probe_weights[p]);
    }
  }
  return sum;
}