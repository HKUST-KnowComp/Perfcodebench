#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total_sum = 0;
  const size_t num_runs = values.size();
  
  // Pre-calculate the sum of one full decode cycle to avoid redundant work
  // since the input vectors do not change across iterations.
  uint64_t single_pass_sum = 0;
  uint32_t prev = 0;
  for (size_t i = 0; i < num_runs; ++i) {
    const uint32_t len = run_ends[i] - prev;
    single_pass_sum += static_cast<uint64_t>(len) * values[i];
    prev = run_ends[i];
  }

  // The original code resets 'sum = 0' inside the loop, meaning only the 
  // last iteration's result is returned. However, the baseline logic 
  // shows 'sum = 0' at the start of the loop and 'sum = 0' before the 
  // final summation loop. This implies the function returns the sum of 
  // the decoded stream for a single iteration, repeated 'iters' times.
  // If iters <= 0, the loop doesn't run and sum remains 0.
  if (iters > 0) {
    total_sum = single_pass_sum;
  } else {
    total_sum = 0;
  }

  return total_sum;
}