#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  // Preserve baseline semantics: if iters <= 0, no work, return 0.
  if (iters <= 0) return 0;

  const uint32_t* p = deltas.data();
  const std::size_t n = deltas.size();
  if (n == 0) return 0;

  uint64_t value = static_cast<uint64_t>(base);
  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);

  // Manual unroll by 4 to reduce loop overhead while preserving dependency chain.
  for (; i < n4; i += 4) {
    uint64_t v = value + static_cast<uint64_t>(p[i]);
    sum += v;
    v += static_cast<uint64_t>(p[i + 1]);
    sum += v;
    v += static_cast<uint64_t>(p[i + 2]);
    sum += v;
    v += static_cast<uint64_t>(p[i + 3]);
    sum += v;
    value = v;
  }
  for (; i < n; ++i) {
    value += static_cast<uint64_t>(p[i]);
    sum += value;
  }

  return sum;
}
