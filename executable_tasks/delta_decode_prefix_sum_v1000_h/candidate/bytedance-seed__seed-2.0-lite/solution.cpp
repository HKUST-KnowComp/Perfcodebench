#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_value = base;
    sum = 0;
    for (uint32_t delta : deltas) {
      current_value += delta;
      sum += current_value;
    }
  }
  return sum;
}