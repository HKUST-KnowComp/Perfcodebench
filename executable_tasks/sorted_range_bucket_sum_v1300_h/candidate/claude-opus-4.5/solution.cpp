#include "interface.h"

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
    const uint32_t* b = b_begin;
    for (const uint32_t* p = p_begin; p != p_end; ++p) {
      uint32_t probe = *p;
      // Advance b while *b <= probe (upper_bound semantics: first element > probe)
      while (b != b_end && *b <= probe) {
        ++b;
      }
      sum += static_cast<uint64_t>(b - b_begin);
    }
  }
  return sum;
}