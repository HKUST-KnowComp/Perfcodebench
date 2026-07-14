#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters <= 0) {
    return 0;
  }
  uint64_t total_sum = 0;
  uint64_t current_value = base;
  for (uint32_t d : deltas) {
    current_value += d;
    total_sum += current_value;
  }
  return total_sum;
}