#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total_sum = 0;
  
  // Pre-calculate the run lengths to avoid repeated subtraction in the inner loop
  // and to minimize work inside the 'iters' loop.
  // However, since 'iters' is part of the performance metric, we must ensure
  // the logic inside the loop is as tight as possible.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    uint32_t prev_end = 0;
    size_t n = values.size();
    
    for (size_t i = 0; i < n; ++i) {
      uint32_t current_end = run_ends[i];
      uint32_t run_len = current_end - prev_end;
      current_iter_sum += static_cast<uint64_t>(values[i]) * run_len;
      prev_end = current_end;
    }
    total_sum = current_iter_sum;
  }
  
  return total_sum;
}