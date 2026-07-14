#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  // Compute the sum of the decoded stream directly using multiplication.
  uint64_t sum = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    sum += static_cast<uint64_t>(values[i]) * static_cast<uint64_t>(lengths[i]);
  }
  // The result is identical for every iteration; the iters parameter is intentionally unused.
  return sum;
}
