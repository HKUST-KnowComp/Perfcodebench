#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = values.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    uint32_t prev = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t current_end = run_ends[i];
      uint32_t len = current_end - prev;
      current_iter_sum += static_cast<uint64_t>(values[i]) * len;
      prev = current_end;
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}