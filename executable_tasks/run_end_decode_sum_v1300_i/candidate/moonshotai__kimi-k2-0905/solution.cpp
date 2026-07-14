#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t run_sum = 0;
    uint32_t prev = 0;
    const size_t n = values.size();
    for (size_t i = 0; i < n; ++i) {
      const uint32_t len = run_ends[i] - prev;
      run_sum += static_cast<uint64_t>(len) * values[i];
      prev = run_ends[i];
    }
    sum = run_sum;
  }
  return sum;
}