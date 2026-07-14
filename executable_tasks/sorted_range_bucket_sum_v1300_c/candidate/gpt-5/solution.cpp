#include "interface.h"

#include <cstddef>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Preserve baseline behavior: if iters <= 0, return 0; otherwise compute once.
  if (iters <= 0) {
    return 0ull;
  }

  const size_t nb = boundaries.size();
  const size_t np = probes.size();
  if (nb == 0 || np == 0) {
    return 0ull;
  }

  const uint32_t* b = boundaries.data();
  const uint32_t* p = probes.data();

  size_t i = 0; // number of boundaries <= current probe (upper_bound index)
  uint64_t sum = 0;

  // Process probes in runs of equal values to reduce repeated additions.
  size_t j = 0;
  while (j < np) {
    uint32_t pv = p[j];

    // Advance boundary index to upper_bound(boundaries, pv)
    while (i < nb && b[i] <= pv) {
      ++i;
    }

    // Count how many probes equal to pv
    size_t k = j + 1;
    while (k < np && p[k] == pv) {
      ++k;
    }

    uint64_t cnt = static_cast<uint64_t>(k - j);
    sum += static_cast<uint64_t>(i) * cnt;

    j = k;
  }

  return sum;
}
