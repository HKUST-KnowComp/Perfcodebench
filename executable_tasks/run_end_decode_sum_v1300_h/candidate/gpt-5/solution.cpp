#include "interface.h"

#include <vector>
#include <cstddef>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Match baseline behavior: if no iterations, return 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = values.size();
  if (n == 0) {
    return 0ULL;
  }

  uint64_t s0 = 0ULL;
  uint64_t s1 = 0ULL;
  uint32_t prev = 0U;

  std::size_t i = 0;
  // Unroll by 4 to reduce loop overhead and expose ILP.
  for (; i + 4 <= n; i += 4) {
    const uint32_t e0 = run_ends[i];
    const uint32_t e1 = run_ends[i + 1];
    const uint32_t e2 = run_ends[i + 2];
    const uint32_t e3 = run_ends[i + 3];

    const uint32_t l0 = e0 - prev;
    const uint32_t l1 = e1 - e0;
    const uint32_t l2 = e2 - e1;
    const uint32_t l3 = e3 - e2;

    s0 += static_cast<uint64_t>(l0) * static_cast<uint64_t>(values[i]);
    s1 += static_cast<uint64_t>(l1) * static_cast<uint64_t>(values[i + 1]);
    s0 += static_cast<uint64_t>(l2) * static_cast<uint64_t>(values[i + 2]);
    s1 += static_cast<uint64_t>(l3) * static_cast<uint64_t>(values[i + 3]);

    prev = e3;
  }

  for (; i < n; ++i) {
    const uint32_t e = run_ends[i];
    const uint32_t l = e - prev;
    s0 += static_cast<uint64_t>(l) * static_cast<uint64_t>(values[i]);
    prev = e;
  }

  return s0 + s1;
}
