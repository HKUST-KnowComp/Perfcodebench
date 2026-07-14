#include "interface.h"

#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t n_boundaries = boundaries.size();
  const size_t n_probes = probes.size();
  
  if (n_probes == 0) return 0;
  
  const uint32_t* __restrict__ b_ptr = boundaries.data();
  const uint32_t* __restrict__ p_ptr = probes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    
    // Linear merge: both arrays are sorted
    // For each probe, find how many boundaries are <= probe
    // Since probes are sorted, we only need to advance b_idx forward
    for (size_t p_idx = 0; p_idx < n_probes; ++p_idx) {
      uint32_t probe = p_ptr[p_idx];
      
      // Advance boundary index while boundary <= probe
      // upper_bound finds first element > probe, so we want count of elements <= probe
      while (b_idx < n_boundaries && b_ptr[b_idx] <= probe) {
        ++b_idx;
      }
      
      sum += b_idx;
    }
  }
  return sum;
}