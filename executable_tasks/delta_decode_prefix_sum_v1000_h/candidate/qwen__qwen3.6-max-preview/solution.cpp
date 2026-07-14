#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  uint64_t current = base;
  for (uint32_t d : deltas) {
    current += d;
    sum += current;
  }
  return sum;
}