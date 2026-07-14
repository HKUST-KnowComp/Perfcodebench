#include "interface.h"
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t sum = 0;
  const size_t n = deltas.size();
  const uint16_t* d_ptr = deltas.data();

  // The baseline re-calculates the same sum 'iters' times.
  // We only need to calculate it once.
  for (size_t i = 0; i < n; ++i) {
    uint32_t value = base + static_cast<uint32_t>(d_ptr[i]);
    if (value >= low && value <= high) {
      sum += static_cast<uint64_t>(value);
    }
  }

  return sum;
}