#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total_sum = 0;
  
  // Pre-calculate the sum for one iteration to avoid redundant work if iters > 1
  // However, the task implies we must simulate the loop structure.
  // Since the sum is reset every iteration in the baseline, we only care about the last iteration's sum.
  // But to be safe and follow the logic: the baseline returns the sum of the LAST iteration.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    uint32_t prev_end = 0;
    size_t n = values.size();
    
    for (size_t i = 0; i < n; ++i) {
      uint32_t current_end = run_ends[i];
      uint32_t run_length = current_end - prev_end;
      current_iter_sum += static_cast<uint64_t>(values[i]) * run_length;
      prev_end = current_end;
    }
    total_sum = current_iter_sum;
  }
  
  return total_sum;
}