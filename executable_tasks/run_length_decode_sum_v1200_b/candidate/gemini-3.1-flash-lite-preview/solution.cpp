#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = values.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      current_iter_sum += static_cast<uint64_t>(values[i]) * static_cast<uint64_t>(lengths[i]);
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}