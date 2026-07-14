#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Preserve baseline behavior: if iters == 0, return 0
  if (iters <= 0) {
    return 0;
  }

  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();
  const std::size_t n = values.size();

  uint64_t sum = 0;
  uint32_t prev = 0;

  // Unroll by 4 for better ILP
  std::size_t i = 0;
  const std::size_t limit = n & static_cast<std::size_t>(~3ULL);
  for (; i < limit; i += 4) {
    uint32_t re0 = re[i];
    uint32_t len0 = re0 - prev;
    uint64_t v0 = val[i];

    uint32_t re1 = re[i + 1];
    uint32_t len1 = re1 - re0;
    uint64_t v1 = val[i + 1];

    uint32_t re2 = re[i + 2];
    uint32_t len2 = re2 - re1;
    uint64_t v2 = val[i + 2];

    uint32_t re3 = re[i + 3];
    uint32_t len3 = re3 - re2;
    uint64_t v3 = val[i + 3];

    sum += v0 * static_cast<uint64_t>(len0);
    sum += v1 * static_cast<uint64_t>(len1);
    sum += v2 * static_cast<uint64_t>(len2);
    sum += v3 * static_cast<uint64_t>(len3);

    prev = re3;
  }

  for (; i < n; ++i) {
    uint32_t rei = re[i];
    uint32_t len = rei - prev;
    sum += static_cast<uint64_t>(val[i]) * static_cast<uint64_t>(len);
    prev = rei;
  }

  return sum;
}
