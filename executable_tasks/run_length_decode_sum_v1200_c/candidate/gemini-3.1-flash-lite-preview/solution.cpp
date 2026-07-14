#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = values.size();
  
  // Pre-calculate the sum of a single pass to avoid redundant work inside the loop
  uint64_t single_pass_sum = 0;
  for (size_t i = 0; i < n; ++i) {
    single_pass_sum += static_cast<uint64_t>(values[i]) * lengths[i];
  }

  // The result is simply the single pass sum multiplied by the number of iterations
  total_sum = single_pass_sum * static_cast<uint64_t>(iters);
  
  return total_sum;
}