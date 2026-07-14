#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters <= 0 || deltas.empty()) return 0;
  uint64_t n = deltas.size();
  uint64_t sum = n * base;
  uint64_t w = n;
  for (uint32_t d : deltas) {
    sum += w * d;
    --w;
  }
  return sum;
}
