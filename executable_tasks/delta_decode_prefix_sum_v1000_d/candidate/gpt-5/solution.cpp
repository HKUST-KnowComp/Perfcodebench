#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  // Preserve baseline behavior: if iters == 0, result is 0
  if (iters <= 0) return 0ULL;

  const std::size_t n = deltas.size();
  if (n == 0) return 0ULL;

  const uint32_t* p = deltas.data();
  uint64_t value = static_cast<uint64_t>(base);
  uint64_t sum = 0ULL;

  // Unrolled loop for better ILP and fewer loop overheads
  std::size_t i = 0;
  const std::size_t limit = n & ~static_cast<std::size_t>(3);
  for (; i < limit; i += 4) {
    value += static_cast<uint64_t>(p[0]); sum += value;
    value += static_cast<uint64_t>(p[1]); sum += value;
    value += static_cast<uint64_t>(p[2]); sum += value;
    value += static_cast<uint64_t>(p[3]); sum += value;
    p += 4;
  }
  for (; i < n; ++i) {
    value += static_cast<uint64_t>(*p++);
    sum += value;
  }

  return sum;
}
