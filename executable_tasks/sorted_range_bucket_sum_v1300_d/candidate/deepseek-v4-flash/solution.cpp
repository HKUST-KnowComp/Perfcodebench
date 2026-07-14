#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  (void)iters; // suppress unused warning
  uint64_t sum = 0;
  size_t j = 0;
  const size_t n = boundaries.size();
  const size_t m = probes.size();
  for (size_t i = 0; i < m; ++i) {
    while (j < n && boundaries[j] <= probes[i]) {
      ++j;
    }
    sum += j; // j is the number of boundaries <= probes[i]
  }
  return sum;
}
