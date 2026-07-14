#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    auto hint = boundaries.begin();
    for (uint32_t probe : probes) {
      hint = std::upper_bound(hint, boundaries.end(), probe);
      sum += static_cast<uint64_t>(hint - boundaries.begin());
    }
  }
  return sum;
}