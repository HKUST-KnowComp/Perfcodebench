#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  size_t idx = 0;
  size_t n = boundaries.size();
  for (uint32_t probe : probes) {
    while (idx < n && boundaries[idx] <= probe) {
      ++idx;
    }
    sum += idx;
  }
  return sum;
}