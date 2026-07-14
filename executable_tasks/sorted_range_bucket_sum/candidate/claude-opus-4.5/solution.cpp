#include "interface.h"

#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  
  const uint32_t* b_begin = boundaries.data();
  const uint32_t* b_end = boundaries.data() + boundaries.size();
  const uint32_t* p_begin = probes.data();
  const uint32_t* p_end = probes.data() + probes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint32_t* b = b_begin;
    
    for (const uint32_t* p = p_begin; p < p_end; ++p) {
      uint32_t probe = *p;
      // Advance boundary pointer while boundary <= probe
      // upper_bound finds first element > probe, so bucket_id = count of elements <= probe
      while (b < b_end && *b <= probe) {
        ++b;
      }
      sum += static_cast<uint64_t>(b - b_begin);
    }
  }
  return sum;
}