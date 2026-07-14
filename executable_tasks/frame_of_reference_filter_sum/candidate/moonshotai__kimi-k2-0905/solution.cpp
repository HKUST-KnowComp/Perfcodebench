#include "interface.h"
#include <vector>

namespace {
#ifdef __GNUC__
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define unlikely(x) (x)
#endif
} // namespace

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {

  if (unlikely(iters <= 0)) return 0;

  uint64_t sum = 0;
  const uint16_t* __restrict d = deltas.data();
  const size_t n = deltas.size();

  for (size_t i = 0; i < n; ++i) {
    const uint32_t value = base + static_cast<uint32_t>(d[i]);
    if (value >= low && value <= high) {
      sum += value;
    }
  }

  return sum * static_cast<uint64_t>(iters);
}