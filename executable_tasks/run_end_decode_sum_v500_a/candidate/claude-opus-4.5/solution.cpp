#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = values.size();
  const uint32_t* re = run_ends.data();
  const uint32_t* v = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    uint32_t prev = 0;
    std::size_t i = 0;

    // Process 4 runs at a time
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      uint32_t e0 = re[i];
      uint32_t e1 = re[i + 1];
      uint32_t e2 = re[i + 2];
      uint32_t e3 = re[i + 3];

      uint64_t len0 = e0 - prev;
      uint64_t len1 = e1 - e0;
      uint64_t len2 = e2 - e1;
      uint64_t len3 = e3 - e2;

      s += len0 * v[i];
      s += len1 * v[i + 1];
      s += len2 * v[i + 2];
      s += len3 * v[i + 3];

      prev = e3;
    }

    // Handle remaining runs
    for (; i < n; ++i) {
      uint32_t e = re[i];
      uint64_t len = e - prev;
      s += len * v[i];
      prev = e;
    }

    sum = s;
  }
  return sum;
}