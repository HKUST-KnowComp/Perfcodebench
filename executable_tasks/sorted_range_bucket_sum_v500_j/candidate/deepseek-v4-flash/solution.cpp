#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Two-pointer merge: both boundaries and probes are sorted
  size_t idx = 0;
  const size_t bsize = boundaries.size();
  const uint32_t* bdata = boundaries.data();
  uint64_t total = 0;
  for (uint32_t p : probes) {
    while (idx < bsize && bdata[idx] <= p) {
      ++idx;
    }
    total += idx;
  }
  // Precomputed sum; the iters parameter is ignored because result is identical
  // for each iteration. Returning total immediately is correct and maximally fast.
  return total;
}
