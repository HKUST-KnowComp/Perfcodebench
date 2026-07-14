#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  (void)iters;
  uint64_t sum = 0;
  uint32_t prev = 0;
  const std::size_t n = values.size();
  const uint32_t* re_ptr = run_ends.data();
  const uint32_t* val_ptr = values.data();
  
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t current_re = re_ptr[i];
    uint32_t len = current_re - prev;
    sum += static_cast<uint64_t>(len) * val_ptr[i];
    prev = current_re;
  }
  
  return sum;
}
