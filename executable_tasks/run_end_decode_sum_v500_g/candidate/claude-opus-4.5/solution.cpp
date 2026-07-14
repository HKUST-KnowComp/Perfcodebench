#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = values.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint32_t prev = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t run_end = run_ends[i];
      const uint64_t len = run_end - prev;
      sum += len * static_cast<uint64_t>(values[i]);
      prev = run_end;
    }
  }
  return sum;
}