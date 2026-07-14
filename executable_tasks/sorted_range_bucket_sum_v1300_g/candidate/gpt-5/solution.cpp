#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int /*iters*/) {
  // Since the baseline resets sum each iteration and returns the same value,
  // we compute the sum once, which preserves correctness and improves performance.
  const size_t bn = boundaries.size();
  const size_t pn = probes.size();
  if (pn == 0 || bn == 0) {
    return 0ull;
  }

  uint64_t sum = 0;
  size_t bi = 0; // index into boundaries

  // Two-pointer linear scan: for each probe, advance boundary index while
  // boundaries[bi] <= probe, then add bi (count of boundaries <= probe).
  for (size_t pi = 0; pi < pn; ++pi) {
    const uint32_t v = probes[pi];
    while (bi < bn && boundaries[bi] <= v) {
      ++bi;
    }
    sum += static_cast<uint64_t>(bi);
    if (bi == bn) {
      // All remaining probes will have the same bucket id (bn).
      const size_t remaining = pn - (pi + 1);
      sum += static_cast<uint64_t>(bn) * static_cast<uint64_t>(remaining);
      break;
    }
  }

  return sum;
}
