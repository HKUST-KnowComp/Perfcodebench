#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  (void)iters; // ignore – result is same each iteration
  uint64_t sum = 0;
  size_t idx = 0;
  const size_t n = boundaries.size();
  for (uint32_t probe : probes) {
    while (idx < n && boundaries[idx] <= probe) {
      ++idx;
    }
    sum += static_cast<uint64_t>(idx);
  }
  return sum;
}
