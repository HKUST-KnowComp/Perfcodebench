#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t n = deltas.size();
  uint64_t sum_per_iter = n * base;
  for (size_t j = 0; j < n; ++j) {
    sum_per_iter += static_cast<uint64_t>(deltas[j]) * (n - j);
  }
  return sum_per_iter * static_cast<uint64_t>(iters);
}