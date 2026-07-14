#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < values.size(); ++i) {
    sum += static_cast<uint64_t>(values[i]) * lengths[i];
  }
  return sum;
}