#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  uint32_t prev = 0;
  for (std::size_t i = 0; i < values.size(); ++i) {
    uint64_t len = run_ends[i] - prev;
    sum += len * values[i];
    prev = run_ends[i];
  }
  return sum;
}
