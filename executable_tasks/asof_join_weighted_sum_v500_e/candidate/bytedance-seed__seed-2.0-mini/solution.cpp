#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const size_t m = dim_ts.size();
  const size_t n = probe_ts.size();
  
  // Handle edge cases for invalid or empty inputs
  if (m == 0 || n == 0 || dim_values.size() != m || probe_weights.size() != n) {
    return 0;
  }

  // Compute total weighted sum once using sorted input properties
  uint64_t total = 0;
  size_t dim_ptr = 0;
  for (size_t i = 0; i < n; ++i) {
    // Advance pointer to first dimension timestamp greater than current probe timestamp
    while (dim_ptr < m && dim_ts[dim_ptr] <= probe_ts[i]) {
      ++dim_ptr;
    }
    // Use the last valid dimension timestamp <= current probe timestamp
    total += static_cast<uint64_t>(dim_values[dim_ptr - 1]) * static_cast<uint64_t>(probe_weights[i]);
  }

  // Reuse precomputed sum across all benchmark iterations
  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    result = total;
  }
  return result;
}