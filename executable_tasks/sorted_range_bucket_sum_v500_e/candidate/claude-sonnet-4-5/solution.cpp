#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t b_size = boundaries.size();
  const size_t p_size = probes.size();
  
  if (b_size == 0 || p_size == 0) {
    return 0;
  }
  
  const uint32_t* b_ptr = boundaries.data();
  const uint32_t* p_ptr = probes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    
    // Process probes in chunks of 4 for better ILP
    size_t i = 0;
    for (; i + 3 < p_size; i += 4) {
      uint32_t probe0 = p_ptr[i];
      uint32_t probe1 = p_ptr[i + 1];
      uint32_t probe2 = p_ptr[i + 2];
      uint32_t probe3 = p_ptr[i + 3];
      
      // Advance boundary index for probe0
      while (b_idx < b_size && b_ptr[b_idx] <= probe0) {
        ++b_idx;
      }
      sum += b_idx;
      
      // Advance boundary index for probe1
      while (b_idx < b_size && b_ptr[b_idx] <= probe1) {
        ++b_idx;
      }
      sum += b_idx;
      
      // Advance boundary index for probe2
      while (b_idx < b_size && b_ptr[b_idx] <= probe2) {
        ++b_idx;
      }
      sum += b_idx;
      
      // Advance boundary index for probe3
      while (b_idx < b_size && b_ptr[b_idx] <= probe3) {
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