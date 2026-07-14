#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  const std::size_t n = deltas.size();
  if (iters <= 0 || n == 0) return 0;

  const uint32_t min_delta = (low <= base) ? 0u : (low - base);
  const uint32_t max_delta = high - base;

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint16_t* p = deltas.data();
    const uint16_t* const end = p + n;

    for (; p != end; ++p) {
      const uint32_t d = static_cast<uint32_t>(*p);
      if (d >= min_delta && d <= max_delta) {
        local_sum += static_cast<uint64_t>(base + d);
      }
    }

    sum = local_sum;
  }

  return sum;
}
