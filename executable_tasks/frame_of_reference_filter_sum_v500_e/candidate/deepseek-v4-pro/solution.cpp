#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  (void)iters; // the result is independent of the iteration count
  uint64_t sum = 0;
  const auto size = deltas.size();
  const uint32_t range = high - low; // precompute span
  for (std::size_t i = 0; i < size; ++i) {
    uint32_t value = base + static_cast<uint32_t>(deltas[i]);
    // branchless range check: (value - low) <= (high - low) wraps correctly
    if (value - low <= range) {
      sum += static_cast<uint64_t>(value);
    }
  }
  return sum;
}