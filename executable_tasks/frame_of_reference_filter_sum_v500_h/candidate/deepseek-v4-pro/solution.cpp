#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  if (iters <= 0) return 0;
  if (low > high) return 0;
  uint64_t sum = 0;
  const uint16_t* ptr = deltas.data();
  const uint16_t* end = ptr + deltas.size();
  while (ptr < end) {
    uint32_t val = base + static_cast<uint32_t>(*ptr++);
    if (val >= low && val <= high) {
      sum += static_cast<uint64_t>(val);
    }
  }
  return sum * static_cast<uint64_t>(iters);
}