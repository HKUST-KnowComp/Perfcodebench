#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized interval lookup sum.
 * 
 * The original implementation used std::upper_bound for every probe, resulting in O(iters * P * log N) complexity.
 * Since both 'starts' and 'probes' are sorted, we can use a two-pointer approach to achieve O(iters * (P + N)) complexity.
 * 
 * Optimization details:
 * 1. Two-pointer scan: Since probes are sorted, the index of the matching interval only moves forward.
 * 2. Raw pointers: Accessing data via raw pointers avoids potential overhead from std::vector iterator/operator[] checks.
 * 3. Local variables: Keeping the sum and interval index in local variables allows the compiler to keep them in registers.
 */
uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  
  // Unused parameter per baseline
  (void)ends;

  if (probes.empty() || starts.empty()) {
    return 0;
  }

  const uint32_t* __restrict p_starts = starts.data();
  const uint32_t* __restrict p_payloads = payloads.data();
  const uint32_t* __restrict p_probes = probes.data();
  const uint32_t* __restrict p_weights = weights.data();

  const std::size_t n_probes = probes.size();
  const std::size_t n_starts = starts.size();

  uint64_t final_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    std::size_t interval_idx = 0;

    for (std::size_t i = 0; i < n_probes; ++i) {
      const uint32_t probe = p_probes[i];

      // Advance the interval pointer to the last start <= probe.
      // Because probes are sorted, interval_idx never needs to reset.
      while (interval_idx + 1 < n_starts && p_starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }

      // The baseline logic: idx = upper_bound - 1.
      // If probe < starts[0], the baseline results in idx = (size_t)-1.
      // We preserve this behavior for correctness relative to the baseline.
      if (probe < p_starts[0]) {
        // This mimics the (size_t)-1 behavior of the baseline.
        // Note: This will likely cause a crash or UB if it actually happens,
        // but we must match the baseline's logic.
        const std::size_t UB_idx = static_cast<std::size_t>(-1);
        current_sum += static_cast<uint64_t>(p_payloads[UB_idx]) * static_cast<uint64_t>(p_weights[i]);
      } else {
        current_sum += static_cast<uint64_t>(p_payloads[interval_idx]) * static_cast<uint64_t>(p_weights[i]);
      }
    }
    final_sum = current_sum;
  }

  return final_sum;
}
