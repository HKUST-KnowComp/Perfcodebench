#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint64_t value = base;
    for (uint32_t delta : deltas) {
      value += static_cast<uint64_t>(delta);
      sum += value;
    }
  }
  return sum;
}
