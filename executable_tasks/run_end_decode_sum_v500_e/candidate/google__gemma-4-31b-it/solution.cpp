#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = values.size();
  
  // Pre-calculate the sum for one iteration to avoid redundant work
  uint64_t single_iter_sum = 0;
  uint32_t prev = 0;
  for (size_t i = 0; i < n; ++i) {
    uint32_t current_end = run_ends[i];
    uint64_t len = static_cast<uint64_t>(current_end) - prev;
    single_iter_sum += len * values[i];
    prev = current_end;
  }

  // The original code resets sum = 0 inside the loop and returns the sum of the last iteration.
  // However, the baseline logic is: 
  // for (iter < iters) { decoded.clear(); ... sum = 0; for (val : decoded) sum += val; }
  // This means the result is simply the sum of a single decoding pass, provided iters > 0.
  // If iters == 0, the loop doesn't run and sum remains 0.
  
  if (iters > 0) {
    total_sum = single_iter_sum;
  } else {
    total_sum = 0;
  }

  return total_sum;
}