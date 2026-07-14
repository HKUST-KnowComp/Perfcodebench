#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t sum = 0;
  const size_t n = deltas.size();
  const uint32_t* data = deltas.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t value = base;
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      value += static_cast<uint64_t>(data[i]);
      iter_sum += value;
    }
    sum = iter_sum;
  }
  return sum;
}
