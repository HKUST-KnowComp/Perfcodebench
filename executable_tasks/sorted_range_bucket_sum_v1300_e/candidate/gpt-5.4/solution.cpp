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

  const uint32_t* b = boundaries.data();
  const uint32_t* p = probes.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;

    size_t i = 0;

    // Probes smaller than the first boundary map to bucket 0.
    while (i < np && p[i] < b[0]) {
      ++i;
    }

    // Main linear sweep: maintain j = upper_bound(boundaries, probe).
    size_t j = 0;
    for (; i < np; ++i) {
      const uint32_t probe = p[i];
      while (j < nb && b[j] <= probe) {
        ++j;
      }
      s += static_cast<uint64_t>(j);
    }

    sum = s;
  }

  return sum;
}
