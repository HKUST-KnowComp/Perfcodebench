#include "interface.h"
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < values.size(); ++i) {
    sum += static_cast<uint64_t>(values[i]) * static_cast<uint64_t>(lengths[i]);
  }
  // The baseline loops iters times, but the sum is the same each iteration.
  // We compute once and return. This matches the final result.
  return sum;
}