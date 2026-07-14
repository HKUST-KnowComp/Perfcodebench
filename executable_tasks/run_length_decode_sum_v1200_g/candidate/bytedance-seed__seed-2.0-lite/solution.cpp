#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const std::size_t run_count = values.size();
    for (std::size_t i = 0; i < run_count; ++i) {
      sum += static_cast<uint64_t>(values[i]) * lengths[i];
    }
  }
  return sum;
}