#include "interface.h"

#include <cstddef>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const uint32_t* b = boundaries.data();
  const std::size_t nb = boundaries.size();
  const uint32_t* p = probes.data();
  const std::size_t np = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t j = 0; // number of boundaries <= current probe
    for (std::size_t i = 0; i < np; ++i) {
      const uint32_t val = p[i];
      while (j < nb && b[j] <= val) {
        ++j;
      }
      sum += static_cast<uint64_t>(j);
    }
  }
  return sum;
}
