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

  const uint32_t* b = nb ? boundaries.data() : nullptr;
  const uint32_t* p = probes.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    size_t j = 0;

    for (size_t i = 0; i < np; ++i) {
      const uint32_t probe = p[i];
      while (j < nb && b[j] <= probe) {
        ++j;
      }
      local_sum += static_cast<uint64_t>(j);
    }

    sum = local_sum;
  }
  return sum;
}
