#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    const std::size_t n = values.size();
    for (std::size_t i = 0; i < n; ++i) {
      current_sum += static_cast<uint64_t>(values[i]) * lengths[i];
    }
    sum = current_sum;
  }
  return sum;
}