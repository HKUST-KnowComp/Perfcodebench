#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t bound_size = boundaries.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t bound_idx = 0;
    for (uint32_t probe : probes) {
      while (bound_idx < bound_size && boundaries[bound_idx] <= probe) {
        ++bound_idx;
      }
      sum += bound_idx;
    }
  }
  return sum;
}