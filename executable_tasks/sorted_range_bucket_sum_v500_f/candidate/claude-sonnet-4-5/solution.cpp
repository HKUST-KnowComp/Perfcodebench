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
    
    for (size_t p_idx = 0; p_idx < p_size; ++p_idx) {
      uint32_t probe = p_ptr[p_idx];
      
      // Move boundary pointer forward while boundary <= probe
      while (b_idx < b_size && b_ptr[b_idx] <= probe) {
        ++b_idx;
      }
      
      sum += b_idx;
    }
  }
  
  return sum;
}
