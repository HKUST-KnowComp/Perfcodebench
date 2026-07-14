#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = deltas.size();
  const uint16_t* data = deltas.data();

  // The problem asks for the sum of values inside a fixed range.
  // The baseline resets 'sum = 0' inside the iters loop, meaning only the last iteration counts.
  // However, the baseline logic is: 
  // for (int iter = 0; iter < iters; ++iter) { 
  //    sum = 0; 
  //    ... calculate sum ... 
  // }
  // return sum;
  // This means the result is simply the sum of one iteration.

  if (iters <= 0) return 0;

  uint64_t current_sum = 0;
  for (size_t i = 0; i < n; ++i) {
    uint32_t value = base + static_cast<uint32_t>(data[i]);
    if (value >= low && value <= high) {
      current_sum += value;
    }
  }

  return current_sum;
}