#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Baseline returns the sum from a single iteration; if iters == 0, it returns 0.
  if (iters <= 0) {
    return 0;
  }

  const uint32_t* bnd = boundaries.empty() ? nullptr : &boundaries[0];
  const uint32_t* prb = probes.empty() ? nullptr : &probes[0];
  const size_t nB = boundaries.size();
  const size_t nP = probes.size();

  uint64_t sum = 0;
  size_t b = 0; // number of boundaries <= current probe

  for (size_t i = 0; i < nP; ++i) {
    const uint32_t p = prb[i];
    while (b < nB && bnd[b] <= p) {
      ++b;
    }
    if (b == nB) {
      // All boundaries are <= current probe; remaining probes contribute nB each.
      sum += static_cast<uint64_t>(nB) * static_cast<uint64_t>(nP - i);
      break;
    }
    sum += static_cast<uint64_t>(b);
  }

  return sum;
}
