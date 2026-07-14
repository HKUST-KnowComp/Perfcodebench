#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const std::size_t num_probes = probes.size();
  const std::size_t num_starts = starts.size();
  
  if (num_probes == 0 || num_starts == 0) return 0;

  uint64_t total_sum = 0;
  
  // Cache pointers to raw data to avoid vector overhead inside loops
  const uint32_t* __restrict p_starts = starts.data();
  const uint32_t* __restrict p_payloads = payloads.data();
  const uint32_t* __restrict p_probes = probes.data();
  const uint32_t* __restrict p_weights = weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    std::size_t start_idx = 0;

    // Since both starts and probes are sorted, we use a two-pointer approach.
    // This reduces complexity from O(M log N) to O(M + N).
    for (std::size_t i = 0; i < num_probes; ++i) {
      const uint32_t probe = p_probes[i];
      
      // Advance start_idx to the largest index such that starts[start_idx] <= probe
      // We assume the baseline logic (upper_bound - 1) implies probe >= starts[0].
      while (start_idx + 1 < num_starts && p_starts[start_idx + 1] <= probe) {
        start_idx++;
      }
      
      current_sum += static_cast<uint64_t>(p_payloads[start_idx]) * static_cast<uint64_t>(p_weights[i]);
    }
    total_sum = current_sum;
  }

  return total_sum;
}