#include "interface.h"

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
  
  uint64_t sum = 0;
  const std::size_t num_intervals = starts.size();
  const std::size_t num_probes = probes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    std::size_t interval_idx = 0;
    
    for (std::size_t i = 0; i < num_probes; ++i) {
      const uint32_t probe = probes[i];
      
      // Advance interval_idx while probe >= current interval start
      while (interval_idx < num_intervals && starts[interval_idx] <= probe) {
        interval_idx++;
      }
      
      // The matching interval is at interval_idx - 1
      if (interval_idx > 0) {
        sum += static_cast<uint64_t>(payloads[interval_idx - 1]) * 
               static_cast<uint64_t>(weights[i]);
      }
    }
  }
  
  return sum;
}