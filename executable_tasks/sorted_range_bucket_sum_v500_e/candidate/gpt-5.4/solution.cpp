#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;

  const size_t bsz = boundaries.size();
  const size_t psz = probes.size();

  if (psz == 0) return 0;
  if (bsz == 0) return 0;

  const uint32_t* const b = boundaries.data();
  const uint32_t* const p = probes.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    size_t i = 0;

    while (i < psz && p[i] < b[0]) {
      ++i;
    }

    if (i == psz) {
      sum = 0;
      continue;
    }

    size_t j = 0;
    for (; i < psz; ++i) {
      const uint32_t probe = p[i];
      while (j < bsz && b[j] <= probe) {
        ++j;
      }
      local_sum += static_cast<uint64_t>(j);
    }

    sum = local_sum;
  }

  return sum;
}
