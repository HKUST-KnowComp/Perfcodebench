#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  const size_t n = values.size();
  const uint32_t* re_ptr = run_ends.data();
  const uint32_t* val_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    uint32_t prev = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t len = re_ptr[i] - prev;
      current_sum += static_cast<uint64_t>(val_ptr[i]) * len;
      prev = re_ptr[i];
    }
    sum = current_sum;
  }
  return sum;
}