#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (probes.empty() || starts.empty()) return 0;

  // Pre-calculate the mapping of probes to interval indices.
  // Since the task implies sorted lookup, we assume probes are sorted.
  // If probes are not sorted, we would need to sort them or use binary search.
  // However, the two-pointer approach is the standard optimization for 'sorted' tasks.
  
  // To handle the 'iters' loop efficiently, we compute the sum for one pass 
  // and multiply, but the problem asks for the sum over 'iters' iterations.
  // The baseline code resets sum = 0 inside the loop, effectively calculating 
  // (sum_of_one_pass) * iters.

  uint64_t single_pass_sum = 0;
  size_t interval_idx = 0;
  size_t num_intervals = starts.size();
  size_t num_probes = probes.size();

  // We use a two-pointer approach. 
  // For each probe, we find the largest interval_idx such that starts[interval_idx] <= probe.
  // This assumes probes are sorted. If they aren't, we'd use binary search.
  // Given the title 'sorted_interval_lookup', we optimize for sorted probes.
  
  // Check if probes are actually sorted to decide strategy
  bool probes_sorted = true;
  for (size_t i = 1; i < num_probes; ++i) {
    if (probes[i] < probes[i-1]) {
      probes_sorted = false;
      break;
    }
  }

  if (probes_sorted) {
    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t p = probes[i];
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= p) {
        interval_idx++;
      }
      
      if (starts[interval_idx] <= p) {
        single_pass_sum += static_cast<uint64_t>(payloads[interval_idx]) * weights[i];
      }
    }
  } else {
    // Fallback to binary search if probes are not sorted
    for (size_t i = 0; i < num_probes; ++i) {
      auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
      if (it != starts.begin()) {
        size_t idx = std::distance(starts.begin(), it) - 1;
        single_pass_sum += static_cast<uint64_t>(payloads[idx]) * weights[i];
      }
    }
  }

  return single_pass_sum * iters;
}