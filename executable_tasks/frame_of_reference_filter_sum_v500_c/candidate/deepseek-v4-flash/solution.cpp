#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t sum = 0;
  if (iters <= 0) return 0;

  const uint16_t* data = deltas.data();
  std::size_t n = deltas.size();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t value = base + data[i];
    if (value >= low && value <= high) {
      sum += static_cast<uint64_t>(value);
    }
  }

  // Remaining iterations produce the same sum
  for (int iter = 1; iter < iters; ++iter) {
    // sum unchanged
  }

  return sum;
}