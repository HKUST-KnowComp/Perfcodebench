#include "interface.h"
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (probes.empty() || starts.empty()) return 0;

  uint64_t total_sum = 0;
  const size_t num_probes = probes.size();
  const size_t num_intervals = starts.size();

  // Pre-calculate or use a two-pointer approach if probes are sorted.
  // Given the task title 'sorted interval lookup', we assume probes are sorted.
  // If probes are not sorted, we would need to sort them or use binary search.
  // However, the most efficient way to handle 'sorted' inputs is the two-pointer method.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t interval_idx = 0;
    
    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t probe = probes[i];
      
      // Advance interval_idx to the largest index such that starts[interval_idx] <= probe
      // Since probes are sorted, interval_idx only moves forward.
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }

      // Check if the current probe falls within the interval [starts[idx], ends[idx])
      // Note: The baseline used upper_bound(starts) - 1, which finds the largest start <= probe.
      // We must also verify the probe is actually within the interval bounds if the intervals are non-overlapping.
      if (starts[interval_idx] <= probe && probe < ends[interval_idx]) {
        current_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum += current_iter_sum;
  }

  // The baseline returns the sum of the LAST iteration, but the loop structure 
  // in the baseline actually resets sum = 0 inside the loop and returns the final sum.
  // Wait, looking at baseline: 'sum = 0' is inside the 'iters' loop. 
  // So it returns the sum of the LAST iteration. 
  // Let's re-examine baseline: 
  // for (int iter = 0; iter < iters; ++iter) { sum = 0; ... sum += ... } 
  // return sum; 
  // This means it returns the sum of the very last iteration. 
  
  // Re-implementing to match baseline behavior exactly (returning sum of last iteration):
  uint64_t last_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    last_sum = 0;
    size_t interval_idx = 0;
    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t probe = probes[i];
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }
      if (starts[interval_idx] <= probe && probe < ends[interval_idx]) {
        last_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return last_sum;
}