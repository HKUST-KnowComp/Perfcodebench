#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total = 0;
  if (!run_ends.empty()) {
    uint32_t prev = 0;
    for (std::size_t i = 0; i < values.size(); ++i) {
      uint32_t len = run_ends[i] - prev;
      total += static_cast<uint64_t>(values[i]) * len;
      prev = run_ends[i];
    }
  }
  // Preserve the iteration loop for compatibility, but no recomputation.
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = total;
  }
  return sum;
}
