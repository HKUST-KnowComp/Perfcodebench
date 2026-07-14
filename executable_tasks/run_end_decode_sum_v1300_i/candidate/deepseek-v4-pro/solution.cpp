#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  uint32_t prev = 0;
  const std::size_t n = values.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t len = run_ends[i] - prev;
    sum += static_cast<uint64_t>(values[i]) * len;
    prev = run_ends[i];
  }
  return sum;
}
