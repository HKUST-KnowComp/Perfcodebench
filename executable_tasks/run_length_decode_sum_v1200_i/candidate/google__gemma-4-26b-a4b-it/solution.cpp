#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t single_pass_sum = 0;
  const std::size_t n = values.size();
  
  // Use a single loop to calculate the sum mathematically.
  // This avoids the O(decoded_length) memory overhead and expansion.
  for (std::size_t i = 0; i < n; ++i) {
    single_pass_sum += static_cast<uint64_t>(values[i]) * lengths[i];
  }

  // The baseline code resets 'sum = 0' inside the 'iters' loop.
  // Therefore, the final value of 'sum' is simply the result of the last iteration.
  return single_pass_sum;
}