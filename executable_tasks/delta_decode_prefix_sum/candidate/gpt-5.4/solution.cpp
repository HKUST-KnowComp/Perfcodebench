#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  const std::size_t n = deltas.size();
  const uint32_t* p = deltas.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t value = static_cast<uint64_t>(base);
    uint64_t local_sum = 0;

    for (std::size_t i = 0; i < n; ++i) {
      value += static_cast<uint64_t>(p[i]);
      local_sum += value;
    }

    sum = local_sum;
  }
  return sum;
}
