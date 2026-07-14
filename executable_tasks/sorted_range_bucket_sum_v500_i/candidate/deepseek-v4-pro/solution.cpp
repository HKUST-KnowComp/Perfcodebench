#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t total = 0;
  size_t bidx = 0;  // number of boundaries <= current probe
  // Assumes probes are sorted in non-decreasing order.
  for (uint32_t p : probes) {
    while (bidx < boundaries.size() && boundaries[bidx] <= p) {
      ++bidx;
    }
    total += bidx;
  }
  return total;
}
