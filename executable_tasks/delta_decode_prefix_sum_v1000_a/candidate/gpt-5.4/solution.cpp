#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  const std::size_t n = deltas.size();
  if (iters <= 0 || n == 0) {
    return 0;
  }

  const uint32_t* p = deltas.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t value = base;
    uint64_t acc = 0;
    std::size_t i = 0;

    for (; i + 4 <= n; i += 4) {
      value += static_cast<uint64_t>(p[i]);
      acc += value;
      value += static_cast<uint64_t>(p[i + 1]);
      acc += value;
      value += static_cast<uint64_t>(p[i + 2]);
      acc += value;
      value += static_cast<uint64_t>(p[i + 3]);
      acc += value;
    }

    for (; i < n; ++i) {
      value += static_cast<uint64_t>(p[i]);
      acc += value;
    }

    sum = acc;
  }

  return sum;
}
