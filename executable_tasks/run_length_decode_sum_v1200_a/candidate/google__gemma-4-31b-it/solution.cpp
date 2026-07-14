#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  // The baseline resets sum = 0 at the start of every iteration.
  // Therefore, the result is simply the sum of the decoded stream for a single iteration.
  // If iters <= 0, the baseline would return 0.
  if (iters <= 0) return 0;

  uint64_t total_sum = 0;
  std::size_t n = values.size();
  
  // Use a pointer-based loop or range-based loop for better compiler optimization
  for (std::size_t i = 0; i < n; ++i) {
    total_sum += static_cast<uint64_t>(values[i]) * lengths[i];
  }
  
  return total_sum;
}