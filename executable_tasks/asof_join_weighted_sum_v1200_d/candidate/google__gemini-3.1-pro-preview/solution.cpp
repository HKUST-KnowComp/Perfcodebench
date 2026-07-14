#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized as-of join weighted sum.
 * 
 * The task specifies that probe timestamps are sorted. This allows us to use a 
 * two-pointer approach (linear scan) instead of binary searching for every probe.
 * Since the dimension table (dim_ts) is large, a single pass through both arrays
 * is significantly more cache-friendly and reduces the complexity from O(P log D) to O(P + D).
 */
uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  if (probe_ts.empty() || dim_ts.empty()) return 0;

  const size_t p_size = probe_ts.size();
  const size_t d_size = dim_ts.size();
  const uint32_t* d_ts_ptr = dim_ts.data();
  const uint32_t* d_val_ptr = dim_values.data();
  const uint32_t* p_ts_ptr = probe_ts.data();
  const uint32_t* p_w_ptr = probe_weights.data();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t dim_idx = 0;

    // Process probes. Since probe_ts is sorted, dim_idx only moves forward.
    for (size_t i = 0; i < p_size; ++i) {
      const uint32_t p_ts = p_ts_ptr[i];

      // Advance dim_idx to the largest index such that dim_ts[dim_idx] <= p_ts.
      // We check dim_idx + 1 to see if we can move forward.
      while (dim_idx + 1 < d_size && d_ts_ptr[dim_idx + 1] <= p_ts) {
        dim_idx++;
      }

      // The as-of join logic: dim_values[dim_idx] is the value associated with 
      // the largest dim_ts[j] <= probe_ts[i].
      // Note: The baseline assumes dim_ts[0] is always <= any probe_ts, 
      // or handles the -1 index case implicitly via unsigned wrap/logic. 
      // We follow the baseline's logic of using the found index.
      current_iter_sum += static_cast<uint64_t>(d_val_ptr[dim_idx]) * static_cast<uint64_t>(p_w_ptr[i]);
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}
