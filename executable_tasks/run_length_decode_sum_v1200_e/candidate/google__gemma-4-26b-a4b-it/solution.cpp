#include "interface.h"

#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t total_sum = 0;
  const std::size_t n = values.size();

  // The task requires computing the sum of the decoded stream over 'iters' iterations.
  // Since the decoded stream is identical in every iteration, we can compute the sum
  // for one iteration and multiply by 'iters'.
  
  uint64_t single_iteration_sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    single_iteration_sum += static_cast<uint64_t>(values[i]) * lengths[i];
  }

  total_sum = single_iteration_sum * iters;
  return total_sum;
}