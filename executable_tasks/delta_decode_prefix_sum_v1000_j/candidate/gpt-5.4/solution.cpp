#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  const std::size_t n = deltas.size();
  if (iters <= 0 || n == 0) {
    return 0;
  }

  uint64_t sum = 0;
  const uint64_t base64 = static_cast<uint64_t>(base);
  const uint32_t* p = deltas.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t value = base64;
    uint64_t acc = 0;

    std::size_t i = 0;
    const std::size_t unroll = n & ~std::size_t(3);
    for (; i < unroll; i += 4) {
      value += static_cast<uint64_t>(p[i + 0]);
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
