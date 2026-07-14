#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  if (iters <= 0 || n == 0) return 0;

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    uint32_t prev = 0;

    const uint32_t* re = run_ends.data();
    const uint32_t* val = values.data();
    const uint32_t* re_end = re + n;

    while (re != re_end) {
      const uint32_t end = *re++;
      const uint32_t v = *val++;
      local_sum += static_cast<uint64_t>(end - prev) * static_cast<uint64_t>(v);
      prev = end;
    }

    sum = local_sum;
  }
  return sum;
}
