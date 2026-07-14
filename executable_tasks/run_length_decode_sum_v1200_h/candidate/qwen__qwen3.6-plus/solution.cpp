#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  const size_t n = values.size();
  for (size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(values[i]) * lengths[i];
  }
  (void)iters;
  return sum;
}
