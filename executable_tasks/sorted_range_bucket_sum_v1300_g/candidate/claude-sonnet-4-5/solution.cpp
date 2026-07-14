#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  const size_t n_boundaries = boundaries.size();
  const size_t n_probes = probes.size();
  
  if (n_probes == 0 || iters == 0) return 0;
  
  const uint32_t* __restrict__ bounds_ptr = boundaries.data();
  const uint32_t* __restrict__ probes_ptr = probes.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t bound_idx = 0;
    
    // Process probes in chunks of 4 for better ILP
    size_t i = 0;
    const size_t n_chunks = n_probes / 4;
    
    for (size_t chunk = 0; chunk < n_chunks; ++chunk) {
      uint32_t p0 = probes_ptr[i];
      uint32_t p1 = probes_ptr[i + 1];
      uint32_t p2 = probes_ptr[i + 2];
      uint32_t p3 = probes_ptr[i + 3];
      
      // Advance boundary for p0
      while (bound_idx < n_boundaries && bounds_ptr[bound_idx] <= p0) {
        ++bound_idx;
      }
      sum += bound_idx;
      
      // Advance boundary for p1
      while (bound_idx < n_boundaries && bounds_ptr[bound_idx] <= p1) {
        ++bound_idx;
      }
      sum += bound_idx;
      
      // Advance boundary for p2
      while (bound_idx < n_boundaries && bounds_ptr[bound_idx] <= p2) {
        ++bound_idx;
      }
      sum += bound_idx;
      
      // Advance boundary for p3
      while (bound_idx < n_boundaries && bounds_ptr[bound_idx] <= p3) {
        ++bound_idx;
      }
      sum += bound_idx;
      
      i += 4;
    }
    
    // Handle remaining probes
    for (; i < n_probes; ++i) {
      uint32_t probe = probes_ptr[i];
      while (bound_idx < n_boundaries && bounds_ptr[bound_idx] <= probe) {
        ++bound_idx;
      }
      sum += bound_idx;
    }
  }
  
  return sum;
}