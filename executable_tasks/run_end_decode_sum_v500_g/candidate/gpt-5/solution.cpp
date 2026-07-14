#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Preserve baseline behavior: if iters <= 0, no work is done and sum remains 0.
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = values.size();
  if (n == 0) {
    return 0;
  }

  const uint32_t* ends = run_ends.data();
  const uint32_t* vals = values.data();

  uint64_t sum = 0;
  uint32_t prev = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t end = ends[i];
    const uint32_t len = end - prev; // run length for this value
    sum += static_cast<uint64_t>(vals[i]) * static_cast<uint64_t>(len);
    prev = end;
  }

  return sum;
}
