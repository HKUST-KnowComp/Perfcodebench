#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  size_t b = 0;
  size_t n_boundaries = boundaries.size();
  for (uint32_t probe : probes) {
    while (b < n_boundaries && boundaries[b] <= probe) {
      ++b;
    }
    sum += b;
  }
  return sum;
}