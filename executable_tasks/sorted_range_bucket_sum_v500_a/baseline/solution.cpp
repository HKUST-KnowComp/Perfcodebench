#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint32_t probe : probes) {
      sum += static_cast<uint64_t>(
          std::upper_bound(boundaries.begin(), boundaries.end(), probe) - boundaries.begin());
    }
  }
  return sum;
}
