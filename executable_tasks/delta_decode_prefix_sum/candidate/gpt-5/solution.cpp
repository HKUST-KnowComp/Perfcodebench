#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  // Match baseline behavior: if no iterations, return 0
  if (iters <= 0) return 0;

  const uint32_t* p = deltas.data();
  const std::size_t n = deltas.size();

  uint64_t value = static_cast<uint64_t>(base);
  uint64_t sum = 0;

  // Unrolled fused decode + sum
  std::size_t i = 0;
  const std::size_t limit = n & static_cast<std::size_t>(~3ULL);
  for (; i < limit; i += 4) {
    value += static_cast<uint64_t>(p[i]);
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
