#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  const std::size_t n = deltas.size();
  if (iters <= 0 || n == 0) {
    return 0;
  }

  const uint32_t* p = deltas.data();
  const uint64_t base64 = static_cast<uint64_t>(base);
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t prefix = base64;
    uint64_t total = 0;

    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
      prefix += static_cast<uint64_t>(p[i + 0]);
      total += prefix;
      prefix += static_cast<uint64_t>(p[i + 1]);
      total += prefix;
      prefix += static_cast<uint64_t>(p[i + 2]);
      total += prefix;
      prefix += static_cast<uint64_t>(p[i + 3]);
      total += prefix;
    }
    for (; i < n; ++i) {
      prefix += static_cast<uint64_t>(p[i]);
      total += prefix;
    }

    sum = total;
  }

  return sum;
}
