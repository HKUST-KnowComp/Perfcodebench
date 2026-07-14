#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t n_boundaries = boundaries.size();
  const size_t n_probes = probes.size();
  const uint32_t* __restrict__ b_ptr = boundaries.data();
  const uint32_t* __restrict__ p_ptr = probes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    
    for (size_t i = 0; i < n_probes; ++i) {
      uint32_t probe = p_ptr[i];
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