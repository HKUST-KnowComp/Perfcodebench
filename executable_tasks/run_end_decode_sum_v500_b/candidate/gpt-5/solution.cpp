#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  uint64_t sum = 0ULL;
  uint32_t prev = 0U;

  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();
  const std::size_t n = values.size();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t end = re[i];
    uint32_t len = end - prev;  // assumes valid non-decreasing run_ends; mirrors baseline behavior
    sum += static_cast<uint64_t>(val[i]) * static_cast<uint64_t>(len);
    prev = end;
  }

  return sum;
}
