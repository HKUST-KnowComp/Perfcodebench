#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Match baseline behavior: if iters <= 0, no work is done and result is 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = values.size();
  const uint32_t* re = run_ends.data();
  const uint32_t* vv = values.data();

  uint64_t sum = 0;
  uint32_t prev = 0;

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t cur_end = re[i];
    const uint32_t len = cur_end - prev;  // As in baseline: assumes non-decreasing run_ends
    sum += static_cast<uint64_t>(len) * static_cast<uint64_t>(vv[i]);
    prev = cur_end;
  }

  return sum;
}
