#include "interface.h"

#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  // Match baseline semantics: if iters <= 0, return 0.
  if (iters <= 0) return 0;

  const std::size_t n = values.size();
  const uint32_t* vp = values.data();
  const uint16_t* lp = lengths.data();

  // Use multiple accumulators and unroll to improve ILP and reduce loop overhead.
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);

  for (; i < n4; i += 4) {
    s0 += static_cast<uint64_t>(vp[i])     * static_cast<uint64_t>(lp[i]);
    s1 += static_cast<uint64_t>(vp[i + 1]) * static_cast<uint64_t>(lp[i + 1]);
    s2 += static_cast<uint64_t>(vp[i + 2]) * static_cast<uint64_t>(lp[i + 2]);
    s3 += static_cast<uint64_t>(vp[i + 3]) * static_cast<uint64_t>(lp[i + 3]);
  }

  uint64_t sum = (s0 + s1) + (s2 + s3);

  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(vp[i]) * static_cast<uint64_t>(lp[i]);
  }

  return sum;
}
