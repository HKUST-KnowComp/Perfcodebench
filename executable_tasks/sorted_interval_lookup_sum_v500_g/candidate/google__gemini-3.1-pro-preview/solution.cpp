#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized interval lookup sum.
 * 
 * The baseline implementation uses std::upper_bound (binary search) for every probe,
 * resulting in O(iters * M * log N) complexity where M is the number of probes and 
 * N is the number of intervals.
 * 
 * Since both 'starts' and 'probes' are sorted, we can use a two-pointer approach
 * to find the matching interval for each probe in linear time O(iters * (N + M)).
 * This approach is much more cache-friendly and avoids the overhead of repeated
 * binary searches.
 */
uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  
  // The 'ends' vector is not used in the baseline logic, so we ignore it here as well.
  (void)ends;

  const std::size_t num_probes = probes.size();
  const std::size_t num_starts = starts.size();

  if (num_probes == 0 || num_starts == 0) {
    return 0;
  }

  // Pointers to raw data for faster access
  const uint32_t* __restrict p_starts = starts.data();
  const uint32_t* __restrict p_payloads = payloads.data();
  const uint32_t* __restrict p_probes = probes.data();
  const uint32_t* __restrict p_weights = weights.data();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    std::size_t interval_idx = 0;

    for (std::size_t i = 0; i < num_probes; ++i) {
      const uint32_t probe = p_probes[i];
      
      // Advance the interval index as long as the next interval starts before or at the probe.
      // This mimics the behavior of std::upper_bound(starts.begin(), starts.end(), probe) - 1.
      while (interval_idx + 1 < num_starts && p_starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }

      // Calculate weighted payload sum.
      // Note: If probe < starts[0], the baseline logic (upper_bound - 1) would result in an 
      // underflow of the index. We assume valid input where probes[i] >= starts[0].
      current_iter_sum += static_cast<uint64_t>(p_payloads[interval_idx]) * static_cast<uint64_t>(p_weights[i]);
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}
