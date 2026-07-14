#include "interface.h"

#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  uint64_t total_sum = 0;
  const size_t num_intervals = starts.size();
  const size_t num_probes = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t interval_idx = 0;

    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t probe = probes[i];
      
      // Advance interval_idx to the last interval where starts[idx] <= probe
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }
      
      current_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}