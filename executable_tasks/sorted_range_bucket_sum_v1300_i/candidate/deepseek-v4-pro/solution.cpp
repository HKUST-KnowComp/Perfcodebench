#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  std::size_t bidx = 0;
  const std::size_t bsize = boundaries.size();
  for (std::size_t i = 0; i < probes.size(); ++i) {
    uint32_t p = probes[i];
    while (bidx < bsize && boundaries[bidx] <= p) {
      ++bidx;
    }
    sum += bidx;
  }
  return sum;
}
