#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t n_b = boundaries.size();
  const size_t n_p = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    for (size_t p_idx = 0; p_idx < n_p; ++p_idx) {
      const uint32_t v = probes[p_idx];
      while (b_idx < n_b && boundaries[b_idx] <= v) {
        ++b_idx;
      }
      sum += b_idx;
    }
  }
  return sum;
}