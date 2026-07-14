#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const std::size_t n_probes = probes.size();
  const std::size_t n_intervals = starts.size();
  
  if (n_probes == 0 || n_intervals == 0) return 0;
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t interval_idx = 0;
    
    for (std::size_t i = 0; i < n_probes; ++i) {
      const uint32_t probe = probes[i];
      
      // Move interval_idx forward while starts[interval_idx+1] <= probe
      while (interval_idx + 1 < n_intervals && starts[interval_idx + 1] <= probe) {
        ++interval_idx;
      }
      
      sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
    }
  }
  
  return sum;
}