#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = values.size();
  uint64_t result = 0;

  uint32_t prev = 0;
  const uint32_t* ends = run_ends.data();
  const uint32_t* vals = values.data();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t end = ends[i];
    uint32_t len = end - prev;
    result += static_cast<uint64_t>(vals[i]) * static_cast<uint64_t>(len);
    prev = end;
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = result;
  }
  return sum;
}
