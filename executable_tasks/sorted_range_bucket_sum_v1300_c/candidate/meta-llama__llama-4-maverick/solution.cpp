#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  auto boundary_begin = boundaries.begin();
  auto boundary_end = boundaries.end();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    auto boundary_it = boundary_begin;
    for (uint32_t probe : probes) {
      boundary_it = std::upper_bound(boundary_it, boundary_end, probe);
      sum += static_cast<uint64_t>(boundary_it - boundary_begin);
    }
  }
  return sum;
}