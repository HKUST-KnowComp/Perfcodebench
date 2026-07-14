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
  
  const uint32_t* b_data = boundaries.data();
  const uint32_t* p_data = probes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    
    // Two-pointer approach exploiting sorted property
    for (size_t p_idx = 0; p_idx < p_size; ++p_idx) {
      uint32_t probe = p_data[p_idx];
      
      // Advance boundary pointer while boundary <= probe
      while (b_idx < b_size && b_data[b_idx] <= probe) {
        ++b_idx;
      }
      
      sum += b_idx;
    }
  }
  
  return sum;
}