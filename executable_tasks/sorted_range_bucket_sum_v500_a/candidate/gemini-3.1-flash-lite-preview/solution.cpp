#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t total_sum = 0;
  const size_t num_boundaries = boundaries.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t bucket_idx = 0;

    for (uint32_t probe : probes) {
      // Advance bucket_idx while the probe is greater than or equal to the current boundary
      while (bucket_idx < num_boundaries && probe >= boundaries[bucket_idx]) {
        bucket_idx++;
      }
      current_iter_sum += bucket_idx;
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}