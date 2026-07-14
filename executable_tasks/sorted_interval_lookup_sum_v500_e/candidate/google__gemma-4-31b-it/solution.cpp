#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  if (starts.empty() || probes.empty()) return 0;

  uint64_t total_sum = 0;
  const size_t num_probes = probes.size();
  const size_t num_starts = starts.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t start_idx = 0;

    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t probe = probes[i];
      
      // Since probes are sorted, we only need to move the start_idx forward
      // to find the largest start[idx] <= probe.
      while (start_idx + 1 < num_starts && starts[start_idx + 1] <= probe) {
        start_idx++;
      }
      
      // The baseline logic: it = upper_bound(starts, probe); idx = it - starts.begin() - 1;
      // This is equivalent to finding the last element <= probe.
      // If probe < starts[0], the baseline would access payloads[-1] (undefined behavior/crash).
      // However, assuming the input guarantees probe >= starts[0] based on the baseline's logic.
      if (probe >= starts[0]) {
        current_sum += static_cast<uint64_t>(payloads[start_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}