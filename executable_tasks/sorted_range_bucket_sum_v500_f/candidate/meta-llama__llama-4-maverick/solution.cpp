#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    auto boundaries_begin = boundaries.begin();
    auto boundaries_end = boundaries.end();
    for (uint32_t probe : probes) {
      sum += static_cast<uint64_t>(
          std::upper_bound(boundaries_begin, boundaries_end, probe) - boundaries_begin);
    }
  }
  return sum;
}