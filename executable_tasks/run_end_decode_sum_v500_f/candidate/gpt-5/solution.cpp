#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Match baseline behavior: if iters <= 0, return 0 without computation.
  if (iters <= 0) {
    return 0;
  }

  uint64_t sum = 0;
  uint32_t prev = 0;

  const uint32_t* re = run_ends.data();
  const uint32_t* vals = values.data();
  const std::size_t n = values.size();

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t len = re[i] - prev;
    sum += static_cast<uint64_t>(len) * static_cast<uint64_t>(vals[i]);
    prev = re[i];
  }

  return sum;
}
