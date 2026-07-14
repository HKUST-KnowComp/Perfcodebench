#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas,
                          uint32_t base,
                          int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = deltas.size();
  if (n == 0) return static_cast<uint64_t>(base) * iters;

  // Re-use a single buffer across iterations – no reallocations.
  std::vector<uint64_t> decoded(n);

  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t value = base;
    uint64_t local_sum = 0;

    // Unroll by 4 to reduce loop overhead and keep the pipeline fed.
    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      value += static_cast<uint64_t>(deltas[i + 0]);
      decoded[i + 0] = value;
      local_sum += value;

      value += static_cast<uint64_t>(deltas[i + 1]);
      decoded[i + 1] = value;
      local_sum += value;

      value += static_cast<uint64_t>(deltas[i + 2]);
      decoded[i + 2] = value;
      local_sum += value;

      value += static_cast<uint64_t>(deltas[i + 3]);
      decoded[i + 3] = value;
      local_sum += value;
    }
    // Tail.
    for (; i < n; ++i) {
      value += static_cast<uint64_t>(deltas[i]);
      decoded[i] = value;
      local_sum += value;
    }

    total += local_sum;
  }

  return total;
}