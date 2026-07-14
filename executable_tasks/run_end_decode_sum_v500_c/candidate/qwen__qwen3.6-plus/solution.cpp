#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  (void)iters;
  uint64_t sum = 0;
  uint32_t prev = 0;
  const size_t n = values.size();
  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();
  
  for (size_t i = 0; i < n; ++i) {
    uint32_t len = re[i] - prev;
    sum += (uint64_t)val[i] * len;
    prev = re[i];
  }
  return sum;
}