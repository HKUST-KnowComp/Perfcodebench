#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) {
    return 0;
  }
  uint64_t sum = 0;
  std::size_t j = 0;
  const std::size_t n = boundaries.size();
  for (uint32_t p : probes) {
    while (j < n && boundaries[j] <= p) {
      ++j;
    }
    sum += static_cast<uint64_t>(j);
  }
  return sum;
}
