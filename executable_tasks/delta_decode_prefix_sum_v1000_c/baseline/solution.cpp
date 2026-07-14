#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t sum = 0;
  std::vector<uint64_t> decoded;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.resize(deltas.size());
    uint64_t value = base;
    for (std::size_t i = 0; i < deltas.size(); ++i) {
      value += static_cast<uint64_t>(deltas[i]);
      decoded[i] = value;
    }
    sum = 0;
    for (uint64_t current : decoded) {
      sum += current;
    }
  }
  return sum;
}
