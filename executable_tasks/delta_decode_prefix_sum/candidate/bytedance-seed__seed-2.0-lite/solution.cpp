#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_val = static_cast<uint64_t>(base);
    uint64_t iter_sum = 0;
    for (uint32_t delta : deltas) {
      current_val += delta;
      iter_sum += current_val;
    }
    final_sum = iter_sum;
  }
  return final_sum;
}