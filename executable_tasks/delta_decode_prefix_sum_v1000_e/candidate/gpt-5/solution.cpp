#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters <= 0) return 0;

  const uint32_t* p = deltas.data();
  const std::size_t n = deltas.size();

  uint64_t v = static_cast<uint64_t>(base);
  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    v += static_cast<uint64_t>(p[i + 0]); sum += v;
    v += static_cast<uint64_t>(p[i + 1]); sum += v;
    v += static_cast<uint64_t>(p[i + 2]); sum += v;
    v += static_cast<uint64_t>(p[i + 3]); sum += v;
    v += static_cast<uint64_t>(p[i + 4]); sum += v;
    v += static_cast<uint64_t>(p[i + 5]); sum += v;
    v += static_cast<uint64_t>(p[i + 6]); sum += v;
    v += static_cast<uint64_t>(p[i + 7]); sum += v;
  }
  for (; i < n; ++i) {
    v += static_cast<uint64_t>(p[i]);
    sum += v;
  }

  return sum;
}
