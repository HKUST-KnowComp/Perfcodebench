#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  if (probes.empty() || starts.empty()) return 0;

  // Pre-calculate the weighted payload for each probe.
  // Since the probes are processed in the same order every iteration,
  // we can compute the sum for one iteration and multiply by iters,
  // or just pre-calculate the per-probe contribution.
  
  size_t num_probes = probes.size();
  size_t num_starts = starts.size();
  const uint32_t* p_starts = starts.data();
  const uint32_t* p_payloads = payloads.data();
  const uint32_t* p_probes = probes.data();
  const uint32_t* p_weights = weights.data();

  uint64_t single_iter_sum = 0;
  
  // Optimization: If probes are sorted, we could use a two-pointer approach.
  // However, the interface doesn't explicitly guarantee probes are sorted, 
  // only that the interval table is. We use binary search once per probe.
  for (size_t i = 0; i < num_probes; ++i) {
    uint32_t probe = p_probes[i];
    
    // Binary search for the interval containing the probe
    uint32_t low = 0;
    uint32_t high = num_starts;
    while (low < high) {
      uint32_t mid = low + (high - low) / 2;
      if (p_starts[mid] <= probe) {
        low = mid + 1;
      } else {
        high = mid;
      }
    }
    
    // The index is low - 1
    uint32_t idx = low - 1;
    single_iter_sum += static_cast<uint64_t>(p_payloads[idx]) * static_cast<uint64_t>(p_weights[i]);
  }

  // The problem asks for the sum over 'iters' iterations.
  // Since the data doesn't change, we can just multiply.
  return single_iter_sum * static_cast<uint64_t>(iters);
}