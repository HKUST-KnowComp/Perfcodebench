#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const uint32_t* b_base = boundaries.data();
  const uint32_t* b_end = b_base + boundaries.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint32_t* b_ptr = b_base;
    for (uint32_t p : probes) {
      while (b_ptr != b_end && *b_ptr <= p) {
        ++b_ptr;
      }
      sum += static_cast<uint64_t>(b_ptr - b_base);
    }
  }
  return sum;
}