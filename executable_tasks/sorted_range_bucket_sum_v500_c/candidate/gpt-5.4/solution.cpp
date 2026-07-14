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

    for (size_t j = 0; j < np; ++j) {
      const uint32_t probe = p[j];
      while (i < nb && b[i] <= probe) {
        ++i;
      }
      s += static_cast<uint64_t>(i);
    }

    sum = s;
  }

  return sum;
}
