#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  const size_t nb = boundaries.size();
  const size_t np = probes.size();

  if (iters <= 0 || np == 0) return 0;
  if (nb == 0) return 0;

  uint64_t sum = 0;

  // Both inputs are sorted. Compute bucket ids for all probes in a single
  // linear pass, then reuse the exact same sum for each iteration.
  size_t b = 0;
  for (size_t i = 0; i < np; ++i) {
    const uint32_t probe = probes[i];
    while (b < nb && boundaries[b] <= probe) {
      ++b;
    }
    sum += static_cast<uint64_t>(b);
  }

  return sum * static_cast<uint64_t>(iters);
}
