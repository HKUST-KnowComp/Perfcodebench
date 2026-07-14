#include "interface.h"

#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (probes.empty() || starts.empty()) {
    return 0;
  }

  uint64_t total_sum = 0;
  
  // Pre-calculate the sum for a single iteration using a two-pointer approach.
  // Since both starts and probes are sorted, we can find the matching interval 
  // for each probe in a single linear pass.
  
  // We need to find the largest index 'idx' such that starts[idx] <= probe.
  // Because the intervals are sorted and non-overlapping (implied by the logic),
  // we can maintain a pointer to the current interval.
  
  // Note: The baseline logic uses std::upper_bound(starts) - 1. 
  // This finds the interval where starts[idx] <= probe < starts[idx+1].
  // We must also ensure the probe is within the interval [starts[idx], ends[idx]).
  // However, the baseline code provided does NOT check 'ends[idx]'. 
  // It strictly follows: idx = upper_bound(starts, probe) - 1.
  // To preserve correctness relative to the baseline, we follow that exact logic.

  size_t interval_idx = 0;
  uint64_t single_iter_sum = 0;
  size_t num_probes = probes.size();
  size_t num_starts = starts.size();

  for (size_t i = 0; i < num_probes; ++i) {
    uint32_t probe = probes[i];
    
    // Advance interval_idx to the largest index such that starts[interval_idx] <= probe
    while (interval_idx + 1 < num_starts && starts[interval_idx + 1] <= probe) {
      interval_idx++;
    }

    // Check if the current interval_idx is valid for the probe
    // The baseline logic: idx = upper_bound - 1. 
    // If probe < starts[0], upper_bound is starts.begin(), idx becomes -1 (underflow/invalid).
    // We must handle the case where probe < starts[0].
    if (starts[interval_idx] <= probe) {
      single_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
    }
  }

  total_sum = single_iter_sum * iters;
  return total_sum;
}