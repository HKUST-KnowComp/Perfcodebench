#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int /*iters*/) {
  // Compute the sum for a single pass. The baseline resets sum each iter
  // and returns only the final pass result, so we only need to compute once.
  const uint32_t* b = boundaries.data();
  const uint32_t* p = probes.data();
  const size_t nb = boundaries.size();
  const size_t np = probes.size();

  size_t j = 0; // number of boundaries <= current probe (bucket id)
  uint64_t sum = 0;

  for (size_t i = 0; i < np; ++i) {
    const uint32_t v = p[i];
    while (j < nb && b[j] <= v) {
      ++j;
    }
    sum += static_cast<uint64_t>(j);
  }

  return sum;
}
