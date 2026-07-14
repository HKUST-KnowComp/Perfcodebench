#include "interface.h"

#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  const uint32_t* __restrict__ b_ptr = boundaries.data();
  const uint32_t* __restrict__ p_ptr = probes.data();
  const size_t b_size = boundaries.size();
  const size_t p_size = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    
    // Linear merge: both arrays are sorted
    // For each probe, find first boundary > probe
    size_t i = 0;
    
    // Process 4 probes at a time when possible
    for (; i + 4 <= p_size; i += 4) {
      uint32_t p0 = p_ptr[i];
      uint32_t p1 = p_ptr[i + 1];
      uint32_t p2 = p_ptr[i + 2];
      uint32_t p3 = p_ptr[i + 3];
      
      // Since probes are sorted, p0 <= p1 <= p2 <= p3
      // Advance b_idx for p0
      while (b_idx < b_size && b_ptr[b_idx] <= p0) {
        ++b_idx;
      }
      sum += b_idx;
      
      // Advance for p1 (starting from current b_idx)
      while (b_idx < b_size && b_ptr[b_idx] <= p1) {
        ++b_idx;
      }
      sum += b_idx;
      
      // Advance for p2
      while (b_idx < b_size && b_ptr[b_idx] <= p2) {
        ++b_idx;
      }
      sum += b_idx;
      
      // Advance for p3
      while (b_idx < b_size && b_ptr[b_idx] <= p3) {
        ++b_idx;
      }
      sum += b_idx;
    }
    
    // Handle remaining probes
    for (; i < p_size; ++i) {
      uint32_t probe = p_ptr[i];
      while (b_idx < b_size && b_ptr[b_idx] <= probe) {
        ++b_idx;
      }
      sum += b_idx;
    }
  }
  
  return sum;
}