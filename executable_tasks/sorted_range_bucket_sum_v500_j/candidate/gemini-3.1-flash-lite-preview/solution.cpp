#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = boundaries.size();
  const size_t m = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t b_idx = 0;
    
    for (size_t p_idx = 0; p_idx < m; ++p_idx) {
      uint32_t probe = probes[p_idx];
      while (b_idx < n && boundaries[b_idx] <= probe) {
        b_idx++;
      }
      current_sum += b_idx;
    }
    total_sum = current_sum;
  }
  return total_sum;
}