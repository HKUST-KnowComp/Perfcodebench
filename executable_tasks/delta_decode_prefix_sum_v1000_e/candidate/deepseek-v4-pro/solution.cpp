#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters <= 0) return 0;
  const size_t n = deltas.size();
  uint64_t total = static_cast<uint64_t>(n) * base;
  uint64_t prefix = 0;
  for (size_t i = 0; i < n; ++i) {
    prefix += static_cast<uint64_t>(deltas[i]);
    total += prefix;
  }
  return total;
}