#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t prefix_sum = 0;
    uint64_t value = base;
    for (std::size_t i = 0; i < deltas.size(); ++i) {
      value += static_cast<uint64_t>(deltas[i]);
      prefix_sum += value;
    }
    sum = prefix_sum;
  }
  return sum;
}