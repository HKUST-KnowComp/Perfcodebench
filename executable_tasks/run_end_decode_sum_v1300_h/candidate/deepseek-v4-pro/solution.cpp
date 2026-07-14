#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  uint32_t prev = 0;
  const size_t n = values.size();
  for (size_t i = 0; i < n; ++i) {
    const uint64_t len = static_cast<uint64_t>(run_ends[i] - prev);
    sum += len * static_cast<uint64_t>(values[i]);
    prev = run_ends[i];
  }
  return sum;
}