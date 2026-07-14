#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = values.size();
  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    uint32_t prev = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t end = re[i];
      local_sum += static_cast<uint64_t>(val[i]) *
                   static_cast<uint64_t>(end - prev);
      prev = end;
    }
    sum = local_sum;
  }
  return sum;
}
