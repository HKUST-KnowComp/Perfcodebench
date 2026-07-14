#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t num_boundaries = boundaries.size();
  const size_t num_probes = probes.size();
  
  if (num_probes == 0) {
    return 0;
  }
  
  const uint32_t* __restrict__ b_ptr = boundaries.data();
  const uint32_t* __restrict__ p_ptr = probes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    
    // Linear merge: since both arrays are sorted, we can find bucket ids
    // by advancing through boundaries as we process probes
    for (size_t p_idx = 0; p_idx < num_probes; ++p_idx) {
      uint32_t probe = p_ptr[p_idx];
      
      // Advance boundary index while boundary <= probe
      // This gives us upper_bound behavior (first boundary > probe)
      while (b_idx < num_boundaries && b_ptr[b_idx] <= probe) {
        ++b_idx;
      }
      
      sum += b_idx;
    }
  }
  
  return sum;
}