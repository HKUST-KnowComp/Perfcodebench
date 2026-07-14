#include "interface.h"

#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  const uint32_t* b_begin = boundaries.data();
  const uint32_t* b_end = b_begin + boundaries.size();
  const uint32_t* p_begin = probes.data();
  const uint32_t* p_end = p_begin + probes.size();
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint32_t* b_ptr = b_begin;
    for (const uint32_t* p_ptr = p_begin; p_ptr != p_end; ++p_ptr) {
      uint32_t probe = *p_ptr;
      // Advance boundary pointer while boundary <= probe
      // upper_bound finds first element > probe, so we want count of elements <= probe
      while (b_ptr != b_end && *b_ptr <= probe) {
        ++b_ptr;
      }
      sum += static_cast<uint64_t>(b_ptr - b_begin);
    }
  }
  return sum;
}