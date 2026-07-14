#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Match baseline behavior: if no iterations, return 0.
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = values.size();
  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();

  uint32_t prev = 0;
  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t end = re[i];
    const uint32_t len = end - prev;
    sum += static_cast<uint64_t>(val[i]) * static_cast<uint64_t>(len);
    prev = end;
  }

  return sum;
}
