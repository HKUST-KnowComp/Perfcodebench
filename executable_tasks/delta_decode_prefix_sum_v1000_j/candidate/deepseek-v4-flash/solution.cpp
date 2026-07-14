#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  const size_t n = deltas.size();
  uint64_t sum = static_cast<uint64_t>(n) * static_cast<uint64_t>(base);
  uint64_t weight = static_cast<uint64_t>(n);
  const uint32_t* data = deltas.data();
  for (size_t i = 0; i < n; ++i) {
    sum += weight * static_cast<uint64_t>(data[i]);
    --weight;
  }
  return sum;
}
