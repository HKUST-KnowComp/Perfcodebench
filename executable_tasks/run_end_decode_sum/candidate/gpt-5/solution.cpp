#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Preserve baseline behavior: if iters <= 0, no work, return 0
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = values.size();
  uint64_t s = 0;
  uint32_t prev = 0;

  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t len = re[i] - prev;
    s += static_cast<uint64_t>(len) * static_cast<uint64_t>(val[i]);
    prev = re[i];
  }

  return s;
}
