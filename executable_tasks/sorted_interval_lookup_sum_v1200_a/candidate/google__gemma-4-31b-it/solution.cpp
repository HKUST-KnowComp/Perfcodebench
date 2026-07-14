#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const size_t num_probes = probes.size();
  const size_t num_intervals = starts.size();
  
  if (num_probes == 0 || num_intervals == 0) return 0;

  const uint32_t* p_starts = starts.data();
  const uint32_t* p_payloads = payloads.data();
  const uint32_t* p_probes = probes.data();
  const uint32_t* p_weights = weights.data();

  uint64_t final_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t interval_idx = 0;

    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t probe = p_probes[i];
      
      // Since probes are sorted, we only need to move the interval pointer forward
      while (interval_idx + 1 < num_intervals && p_starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }
      
      current_sum += static_cast<uint64_t>(p_payloads[interval_idx]) * static_cast<uint64_t>(p_weights[i]);
    }
    final_sum = current_sum;
  }

  return final_sum;
}