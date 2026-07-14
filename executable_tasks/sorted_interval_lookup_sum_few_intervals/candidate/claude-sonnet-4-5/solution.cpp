#include "interface.h"

#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();
  
  // Cache pointers for direct access
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (std::size_t i = 0; i < n_probes; ++i) {
      const uint32_t probe = probes_ptr[i];
      
      // Prefetch next probe data if available
      if (i + 1 < n_probes) {
        __builtin_prefetch(&probes_ptr[i + 1], 0, 3);
        __builtin_prefetch(&weights_ptr[i + 1], 0, 3);
      }
      
      // Manual binary search optimized for few intervals
      std::size_t left = 0;
      std::size_t right = n_intervals;
      
      while (left < right) {
        std::size_t mid = left + (right - left) / 2;
        if (starts_ptr[mid] <= probe) {
          left = mid + 1;
        } else {
          right = mid;
        }
      }
      
      const std::size_t idx = left - 1;
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
  
  return sum;
}