#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = values.size();
  const uint32_t* re = run_ends.data();
  const uint32_t* v = values.data();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint32_t prev = 0;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t len = re[i] - prev;
      sum += static_cast<uint64_t>(v[i]) * len;
      prev = re[i];
    }
  }
  return sum;
}