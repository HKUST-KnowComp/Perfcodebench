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
  uint64_t sum = 0;
  
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const std::size_t n_probes = probes.size();
  const std::size_t n_starts = starts.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
    // Unrolled loop for better ILP
    for (; i + 3 < n_probes; i += 4) {
      // Prefetch next iteration data
      if (i + 7 < n_probes) {
        __builtin_prefetch(&probes_ptr[i + 4], 0, 0);
        __builtin_prefetch(&weights_ptr[i + 4], 0, 0);
      }
      
      // Process 4 probes in parallel
      uint32_t probe0 = probes_ptr[i];
      uint32_t probe1 = probes_ptr[i + 1];
      uint32_t probe2 = probes_ptr[i + 2];
      uint32_t probe3 = probes_ptr[i + 3];
      
      const uint32_t* it0 = std::upper_bound(starts_ptr, starts_ptr + n_starts, probe0);
      const uint32_t* it1 = std::upper_bound(starts_ptr, starts_ptr + n_starts, probe1);
      const uint32_t* it2 = std::upper_bound(starts_ptr, starts_ptr + n_starts, probe2);
      const uint32_t* it3 = std::upper_bound(starts_ptr, starts_ptr + n_starts, probe3);
      
      std::size_t idx0 = (it0 - starts_ptr) - 1;
      std::size_t idx1 = (it1 - starts_ptr) - 1;
      std::size_t idx2 = (it2 - starts_ptr) - 1;
      std::size_t idx3 = (it3 - starts_ptr) - 1;
      
      sum += static_cast<uint64_t>(payloads_ptr[idx0]) * static_cast<uint64_t>(weights_ptr[i]);
      sum += static_cast<uint64_t>(payloads_ptr[idx1]) * static_cast<uint64_t>(weights_ptr[i + 1]);
      sum += static_cast<uint64_t>(payloads_ptr[idx2]) * static_cast<uint64_t>(weights_ptr[i + 2]);
      sum += static_cast<uint64_t>(payloads_ptr[idx3]) * static_cast<uint64_t>(weights_ptr[i + 3]);
    }
    
    // Handle remaining probes
    for (; i < n_probes; ++i) {
      const uint32_t* it = std::upper_bound(starts_ptr, starts_ptr + n_starts, probes_ptr[i]);
      const std::size_t idx = (it - starts_ptr) - 1;
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
  return sum;
}