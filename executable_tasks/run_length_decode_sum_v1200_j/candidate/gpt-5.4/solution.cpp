#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  const std::size_t n = values.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll_end = n & ~std::size_t(3);
    for (; i < unroll_end; i += 4) {
      local_sum += static_cast<uint64_t>(values[i]) * static_cast<uint64_t>(lengths[i]);
      local_sum += static_cast<uint64_t>(values[i + 1]) * static_cast<uint64_t>(lengths[i + 1]);
      local_sum += static_cast<uint64_t>(values[i + 2]) * static_cast<uint64_t>(lengths[i + 2]);
      local_sum += static_cast<uint64_t>(values[i + 3]) * static_cast<uint64_t>(lengths[i + 3]);
    }
    for (; i < n; ++i) {
      local_sum += static_cast<uint64_t>(values[i]) * static_cast<uint64_t>(lengths[i]);
    }

    sum = local_sum;
  }

  return sum;
}
