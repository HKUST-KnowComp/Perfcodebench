#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t total = 0;
  size_t n = values.size();
  for (size_t i = 0; i < n; ++i) {
    total += static_cast<uint64_t>(values[i]) * lengths[i];
  }
  return total;
}
