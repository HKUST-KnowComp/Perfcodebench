#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t n_boundaries = boundaries.size();
  const size_t n_probes = probes.size();
  
  if (n_boundaries == 0 || n_probes == 0) {
    return 0;
  }
  
  const uint32_t* bound_ptr = boundaries.data();
  const uint32_t* probe_ptr = probes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t bound_idx = 0;
    
    for (size_t i = 0; i < n_probes; ++i) {
      uint32_t probe = probe_ptr[i];
      
      // Linear scan from current position since probes are sorted
      while (bound_idx < n_boundaries && bound_ptr[bound_idx] <= probe) {
        ++bound_idx;
      }
      
      sum += bound_idx;
    }
  }
  
  return sum;
}