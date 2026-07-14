#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = deltas.size();
  if (n == 0) return 0;

  const uint32_t* p = deltas.data();
  uint64_t sum = 0;
  uint64_t value = static_cast<uint64_t>(base);

  std::size_t i = 0;
  const std::size_t n_unroll = n & ~static_cast<std::size_t>(3);
  for (; i < n_unroll; i += 4) {
    value += static_cast<uint64_t>(p[i + 0]);
    sum += value;
    value += static_cast<uint64_t>(p[i + 1]);
    sum += value;
    value += static_cast<uint64_t>(p[i + 2]);
    sum += value;
    value += static_cast<uint64_t>(p[i + 3]);
    sum += value;
  }
  for (; i < n; ++i) {
    value += static_cast<uint64_t>(p[i]);
    sum += value;
  }

  return sum;
}
