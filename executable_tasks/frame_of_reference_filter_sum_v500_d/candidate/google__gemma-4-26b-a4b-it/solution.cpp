#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = deltas.size();
  const uint16_t* __restrict__ d_ptr = deltas.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    // The loop is structured to be easily auto-vectorized by modern compilers.
    // We process deltas directly to avoid the overhead of writing to and reading from a large vector.
    for (size_t i = 0; i < n; ++i) {
      uint32_t value = base + static_cast<uint32_t>(d_ptr[i]);
      if (value >= low && value <= high) {
        current_iter_sum += static_cast<uint64_t>(value);
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}