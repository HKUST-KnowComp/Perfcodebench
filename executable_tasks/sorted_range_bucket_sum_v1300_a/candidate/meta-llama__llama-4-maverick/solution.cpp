#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    auto boundary_it = boundaries.begin();
    for (uint32_t probe : probes) {
      boundary_it = std::upper_bound(boundary_it, boundaries.end(), probe);
      sum += static_cast<uint64_t>(boundary_it - boundaries.begin());
    }
  }
  return sum;
}