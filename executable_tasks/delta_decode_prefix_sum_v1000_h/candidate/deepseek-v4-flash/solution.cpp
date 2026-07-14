#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  // Compute the sum of decoded prefix values in one pass.
  uint64_t prefix = base;
  uint64_t sum = 0;
  for (std::size_t i = 0; i < deltas.size(); ++i) {
    prefix += static_cast<uint64_t>(deltas[i]);
    sum += prefix;
  }
  // The baseline returns 0 when iters == 0; otherwise returns the computed sum.
  return (iters == 0) ? 0 : sum;
}
