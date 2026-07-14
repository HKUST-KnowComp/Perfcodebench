#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized interval lookup sum.
 * 
 * Since both 'starts' and 'probes' are sorted, we can replace the O(log N) binary search 
 * per probe with a single O(N + P) linear pass (two-pointer approach).
 * 
 * Complexity: O(iters * (starts.size() + probes.size()))
 */
uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  
  // Suppress unused parameter warning for 'ends' as per baseline
  (void)ends;

  if (probes.empty() || starts.empty()) {
    return 0;
  }

  uint64_t total_sum = 0;
  const std::size_t num_probes = probes.size();
  const std::size_t num_starts = starts.size();

  // Cache pointers to raw data to avoid overhead of std::vector::operator[] and size checks
  const uint32_t* __restrict p_starts = starts.data();
  const uint32_t* __restrict p_payloads = payloads.data();
  const uint32_t* __restrict p_probes = probes.data();
  const uint32_t* __restrict p_weights = weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    std::size_t interval_idx = 0;

    for (std::size_t i = 0; i < num_probes; ++i) {
      const uint32_t probe_val = p_probes[i];

      // Advance the interval index as long as the next interval starts before or at the probe.
      // Because probes are sorted, interval_idx only moves forward.
      while (interval_idx + 1 < num_starts && p_starts[interval_idx + 1] <= probe_val) {
        interval_idx++;
      }

      // The baseline logic: idx = upper_bound - 1.
      // This corresponds to the largest index such that starts[idx] <= probe.
      // Note: If probe < starts[0], the baseline behavior (it - starts.begin() - 1) 
      // results in an underflow/invalid index. We assume probes[i] >= starts[0] 
      // based on the baseline's logic structure.
      current_iter_sum += static_cast<uint64_t>(p_payloads[interval_idx]) * static_cast<uint64_t>(p_weights[i]);
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}
