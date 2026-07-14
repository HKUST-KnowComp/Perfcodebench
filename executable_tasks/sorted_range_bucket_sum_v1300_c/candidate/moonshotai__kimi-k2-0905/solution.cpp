#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  const uint32_t*  b = boundaries.data();
  const uint32_t*  p = probes.data();
  const size_t     n = boundaries.size();
  const size_t     m = probes.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    size_t   bucket    = 0;          // boundaries[0..bucket-1] <= probe
    for (size_t i = 0; i < m; ++i) {
      const uint32_t probe = p[i];
      while (bucket < n && b[bucket] <= probe) {
        ++bucket;
      }
      local_sum += bucket;
    }
    sum = local_sum;
  }
  return sum;
}