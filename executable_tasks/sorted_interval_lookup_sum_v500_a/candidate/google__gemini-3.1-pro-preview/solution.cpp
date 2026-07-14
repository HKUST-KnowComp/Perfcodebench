#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized interval lookup sum.
 * 
 * Since both 'starts' and 'probes' are sorted, we can replace the O(log N) binary search 
 * per probe with a two-pointer approach that runs in O(N + P) time per iteration.
 */
uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  
  // Suppress unused parameter warning for 'ends'
  (void)ends;

  const std::size_t n_starts = starts.size();
  const std::size_t n_probes = probes.size();

  if (n_probes == 0 || n_starts == 0) {
    return 0;
  }

  // Use raw pointers to avoid potential overhead of std::vector iterator/access checks
  const uint32_t* __restrict p_starts = starts.data();
  const uint32_t* __restrict p_payloads = payloads.data();
  const uint32_t* __restrict p_probes = probes.data();
  const uint32_t* __restrict p_weights = weights.data();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    std::size_t idx = 0;

    // Two-pointer scan: since probes are sorted, the corresponding index in 'starts'
    // only moves forward.
    for (std::size_t i = 0; i < n_probes; ++i) {
      const uint32_t probe = p_probes[i];

      // Advance idx to the largest index such that p_starts[idx] <= probe.
      // The baseline logic (upper_bound - 1) implies we find the last element <= probe.
      while (idx + 1 < n_starts && p_starts[idx + 1] <= probe) {
        idx++;
      }

      // Calculate weighted payload sum.
      // Note: The baseline code has a potential underflow if probe < starts[0].
      // We assume probe >= starts[0] based on the baseline's behavior.
      current_sum += static_cast<uint64_t>(p_payloads[idx]) * static_cast<uint64_t>(p_weights[i]);
    }
    total_sum = current_sum;
  }

  return total_sum;
}