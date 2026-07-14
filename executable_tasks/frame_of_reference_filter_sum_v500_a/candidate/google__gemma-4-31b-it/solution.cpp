#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t single_pass_sum = 0;
  const uint16_t* data = deltas.data();
  size_t size = deltas.size();

  // The values are constant across all iterations, so we calculate once
  // and multiply by iters at the end.
  for (size_t i = 0; i < size; ++i) {
    uint32_t value = base + static_cast<uint32_t>(data[i]);
    if (value >= low && value <= high) {
      single_pass_sum += value;
    }
  }

  return single_pass_sum * static_cast<uint64_t>(iters);
}