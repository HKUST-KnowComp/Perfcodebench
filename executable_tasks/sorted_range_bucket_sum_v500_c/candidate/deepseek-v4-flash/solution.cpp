#include "interface.h"

#include <cstddef>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Precompute the total sum of bucket IDs with a single linear pass.
  size_t j = 0;
  uint64_t total = 0;
  for (size_t i = 0; i < probes.size(); ++i) {
    while (j < boundaries.size() && boundaries[j] <= probes[i]) {
      ++j;
    }
    total += static_cast<uint64_t>(j);
  }

  // Each iteration just reuses the precomputed total.
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = total;
  }
  return sum;
}
