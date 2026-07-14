#include "interface.h"

#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  const uint32_t* b_data = boundaries.data();
  const size_t b_size = boundaries.size();
  const uint32_t* p_data = probes.data();
  const size_t p_size = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    
    // Linear merge: since both arrays are sorted, we can walk through them together
    // For each probe, advance b_idx while boundaries[b_idx] <= probe
    // The bucket index is then b_idx (number of boundaries <= probe, which equals
    // the number of boundaries strictly less than probe + 1 if equal, matching upper_bound)
    
    for (size_t i = 0; i < p_size; ++i) {
      const uint32_t probe = p_data[i];
      
      // Advance boundary pointer while boundary <= probe
      // upper_bound finds first element > probe, so index = count of elements <= probe
      while (b_idx < b_size && b_data[b_idx] <= probe) {
        ++b_idx;
      }
      
      sum += b_idx;
    }
  }
  
  return sum;
}