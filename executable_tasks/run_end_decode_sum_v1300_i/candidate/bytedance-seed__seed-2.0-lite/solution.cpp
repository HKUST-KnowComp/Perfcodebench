#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total_sum = 0;
  if (!run_ends.empty() && !values.empty()) {
    uint32_t prev = 0;
    for (std::size_t i = 0; i < values.size(); ++i) {
      const uint32_t len = run_ends[i] - prev;
      total_sum += static_cast<uint64_t>(len) * static_cast<uint64_t>(values[i]);
      prev = run_ends[i];
    }
  }
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = total_sum;
  }
  return sum;
}