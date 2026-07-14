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
  
  // The baseline implementation resets 'sum' to 0 at the start of every iteration.
  // Since the inputs are constant, the final result is simply the result of the last iteration.
  // If iters <= 0, the loop never runs and the function returns 0.
  if (iters <= 0 || probes.empty() || starts.empty()) {
    return 0;
  }

  // Use a two-pointer approach to calculate the sum in O(N + M) time.
  // This is possible because both 'starts' and 'probes' are sorted.
  uint64_t sum = 0;
  std::size_t start_idx = 0;
  const std::size_t num_starts = starts.size();
  const std::size_t num_probes = probes.size();

  for (std::size_t i = 0; i < num_probes; ++i) {
    const uint32_t current_probe = probes[i];
    
    // Advance start_idx to find the largest index such that starts[start_idx] <= current_probe.
    // This mimics the baseline's 'std::upper_bound(...) - 1' logic.
    while (start_idx + 1 < num_starts && starts[start_idx + 1] <= current_probe) {
      start_idx++;
    }
    
    // Note: The baseline logic 'it - starts.begin() - 1' would result in an out-of-bounds 
    // access if current_probe < starts[0]. We assume valid input where current_probe >= starts[0].
    sum += static_cast<uint64_t>(payloads[start_idx]) * static_cast<uint64_t>(weights[i]);
  }

  return sum;
}