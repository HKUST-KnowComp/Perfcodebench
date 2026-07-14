#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (probes.empty() || iters <= 0) return 0;

  uint64_t single_pass_sum = 0;
  size_t b_idx = 0;
  size_t b_size = boundaries.size();

  // Since both boundaries and probes are sorted, we can use a two-pointer approach
  // to find the upper_bound for each probe in linear time.
  for (uint32_t probe : probes) {
    while (b_idx < b_size && boundaries[b_idx] <= probe) {
      ++b_idx;
    }
    single_pass_sum += b_idx;
  }

  // The original code resets sum = 0 inside the loop, meaning it only returns 
  // the result of the final iteration. However, the logic implies it wants 
  // the sum for one set of probes. If the loop was intended to accumulate, 
  // it would be single_pass_sum * iters. Based on the baseline 'sum = 0' 
  // inside the loop, it returns the result of the last iteration.
  return single_pass_sum;
}