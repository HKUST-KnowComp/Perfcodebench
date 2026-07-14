#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b = 0;
    const size_t B = boundaries.size();
    for (uint32_t p : probes) {
      while (b < B && boundaries[b] <= p) ++b;
      sum += static_cast<uint64_t>(b);
    }
  }
  return sum;
}