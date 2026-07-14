#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters <= 0) return 0; // preserve baseline behavior

  const size_t n = deltas.size();
  if (n == 0) return 0;

  const uint32_t* p = deltas.data();
  unsigned __int128 sum128 = 0;
  uint64_t value = static_cast<uint64_t>(base);

  size_t i = 0;
  const size_t n4 = n & ~static_cast<size_t>(3);
  for (; i < n4; i += 4) {
    value += p[i + 0]; sum128 += value;
    value += p[i + 1]; sum128 += value;
    value += p[i + 2]; sum128 += value;
    value += p[i + 3]; sum128 += value;
  }
  for (; i < n; ++i) {
    value += p[i];
    sum128 += value;
  }

  return static_cast<uint64_t>(sum128);
}
