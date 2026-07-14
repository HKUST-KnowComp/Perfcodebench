#include "interface.h"

#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  const size_t n = boundaries.size();
  size_t idx = 0;
  for (uint32_t p : probes) {
    while (idx < n && boundaries[idx] <= p) {
      ++idx;
    }
    sum += static_cast<uint64_t>(idx);
  }
  return sum;
}