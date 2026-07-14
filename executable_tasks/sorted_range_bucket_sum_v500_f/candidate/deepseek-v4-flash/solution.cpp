#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters == 0) return 0;
  size_t b = 0;
  uint64_t sum = 0;
  for (uint32_t probe : probes) {
    while (b < boundaries.size() && boundaries[b] <= probe) {
      ++b;
    }
    sum += static_cast<uint64_t>(b);
  }
  return sum;
}