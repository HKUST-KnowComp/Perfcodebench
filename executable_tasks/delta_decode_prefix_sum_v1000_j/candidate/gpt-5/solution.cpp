#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  // Match baseline behavior: if iters <= 0, do nothing and return 0
  if (iters <= 0) return 0;

  const uint32_t* p = deltas.data();
  const std::size_t n = deltas.size();

  uint64_t v = static_cast<uint64_t>(base);
  uint64_t s = 0;

  std::size_t i = 0;
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    v += p[i + 0]; s += v;
    v += p[i + 1]; s += v;
    v += p[i + 2]; s += v;
    v += p[i + 3]; s += v;
    v += p[i + 4]; s += v;
    v += p[i + 5]; s += v;
    v += p[i + 6]; s += v;
    v += p[i + 7]; s += v;
  }
  for (; i < n; ++i) {
    v += p[i];
    s += v;
  }

  return s;
}
