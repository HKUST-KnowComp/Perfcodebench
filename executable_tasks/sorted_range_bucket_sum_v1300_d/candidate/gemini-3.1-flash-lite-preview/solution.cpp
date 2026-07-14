#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n_boundaries = boundaries.size();
  const size_t n_probes = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t b_idx = 0;
    
    for (size_t p_idx = 0; p_idx < n_probes; ++p_idx) {
      uint32_t probe = probes[p_idx];
      // Advance boundary pointer while the current boundary is <= probe
      while (b_idx < n_boundaries && boundaries[b_idx] <= probe) {
        b_idx++;
      }
      current_iter_sum += static_cast<uint64_t>(b_idx);
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}