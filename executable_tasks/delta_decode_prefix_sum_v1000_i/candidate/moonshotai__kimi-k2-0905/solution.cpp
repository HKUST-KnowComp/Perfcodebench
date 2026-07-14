#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t total = 0;
  const std::size_t n = deltas.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t prefix = base;
    uint64_t iter_sum = 0;

    for (std::size_t i = 0; i < n; ++i) {
      prefix += static_cast<uint64_t>(deltas[i]);
      iter_sum += prefix;
    }

    total = iter_sum;  // overwrite each iteration, matching baseline behaviour
  }

  return total;
}