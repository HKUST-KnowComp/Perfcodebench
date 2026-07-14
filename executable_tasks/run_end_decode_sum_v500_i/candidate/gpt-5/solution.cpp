#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Match baseline behavior: if no iterations, return 0
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = values.size();
  if (n == 0) {
    return 0;
  }

  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();

  uint64_t sum = 0;
  // First run length is run_ends[0]
  sum += static_cast<uint64_t>(val[0]) * static_cast<uint64_t>(re[0]);
  for (std::size_t i = 1; i < n; ++i) {
    uint32_t len = re[i] - re[i - 1];
    sum += static_cast<uint64_t>(val[i]) * static_cast<uint64_t>(len);
  }

  return sum;
}
