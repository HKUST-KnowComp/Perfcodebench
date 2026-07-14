#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t n = deltas.size();
  uint64_t sum = n * base;
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(deltas[i]) * (n - i);
  }
  return sum * static_cast<uint64_t>(iters);
}