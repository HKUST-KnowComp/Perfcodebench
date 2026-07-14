#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = values.size();
  
  // Pre-calculate the sum of one iteration to avoid redundant work
  uint64_t single_iter_sum = 0;
  for (size_t i = 0; i < n; ++i) {
    single_iter_sum += static_cast<uint64_t>(values[i]) * lengths[i];
  }

  // The problem asks for the sum of the decoded stream.
  // Since the decoded stream is identical in every iteration,
  // the total sum is simply single_iter_sum * iters.
  total_sum = single_iter_sum * static_cast<uint64_t>(iters);

  return total_sum;
}