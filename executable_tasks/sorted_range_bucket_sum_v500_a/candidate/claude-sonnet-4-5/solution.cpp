#include "interface.h"

#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  const size_t n_boundaries = boundaries.size();
  const size_t n_probes = probes.size();
  
  if (n_probes == 0) return 0;
  if (n_boundaries == 0) {
    return 0;
  }
  
  const uint32_t* __restrict__ bound_ptr = boundaries.data();
  const uint32_t* __restrict__ probe_ptr = probes.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t bound_idx = 0;
    
    // Process probes in chunks of 4 for better ILP
    size_t i = 0;
    const size_t n_unroll = (n_probes / 4) * 4;
    
    for (; i < n_unroll; i += 4) {
      uint32_t p0 = probe_ptr[i];
      uint32_t p1 = probe_ptr[i + 1];
      uint32_t p2 = probe_ptr[i + 2];
      uint32_t p3 = probe_ptr[i + 3];
      
      // Advance boundary for p0
      while (bound_idx < n_boundaries && bound_ptr[bound_idx] <= p0) {
        ++bound_idx;
      }
      sum += bound_idx;
      
      // Advance boundary for p1
      while (bound_idx < n_boundaries && bound_ptr[bound_idx] <= p1) {
        ++bound_idx;
      }
      sum += bound_idx;
      
      // Advance boundary for p2
      while (bound_idx < n_boundaries && bound_ptr[bound_idx] <= p2) {
        ++bound_idx;
      }
      sum += bound_idx;
      
      // Advance boundary for p3
      while (bound_idx < n_boundaries && bound_ptr[bound_idx] <= p3) {
        ++bound_idx;
      }
      sum += bound_idx;
    }
    
    // Handle remaining probes
    for (; i < n_probes; ++i) {
      uint32_t probe = probe_ptr[i];
      while (bound_idx < n_boundaries && bound_ptr[bound_idx] <= probe) {
        ++bound_idx;
      }
      sum += bound_idx;
    }
  }
  
  return sum;
}