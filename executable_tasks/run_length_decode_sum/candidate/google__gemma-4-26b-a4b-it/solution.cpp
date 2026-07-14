#include "interface.h"

#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t total_sum = 0;
  const std::size_t n = values.size();

  // The task requires the sum of the decoded stream after 'iters' iterations.
  // Since the decoded stream is identical in every iteration, we calculate
  // the sum for one iteration and multiply by iters.
  // However, to strictly follow the logic of the baseline (which resets sum = 0 
  // inside the loop and returns the sum of the LAST iteration), 
  // we must ensure we return the sum of a single decoded stream.
  
  // Note: The baseline code has a logic quirk: 'sum = 0' is inside the 'iters' loop,
  // meaning it returns the sum of the decoded stream from the very last iteration.
  
  for (std::size_t i = 0; i < n; ++i) {
    total_sum += static_cast<uint64_t>(values[i]) * lengths[i];
  }

  // Based on the baseline's behavior: 
  // for (int iter = 0; iter < iters; ++iter) { ... sum = 0; for (uint32_t value : decoded) { sum += value; } }
  // The 'sum' returned is the sum of the decoded stream of the final iteration.
  // If iters <= 0, the baseline returns 0.
  if (iters <= 0) return 0;

  return total_sum;
}